#include <string>
#include <string.h>
#include <string_view>
#include <utility>
#include "HTTPError.hpp"
#include "Request.hpp"
#include "Connection.hpp"
#include "CGICreationTask.hpp"
#include "Log.hpp"
#include <unistd.h>
#include <string>
#include <sys/socket.h>

extern char** environ;
using namespace cgi_creation_task;
using std::string;

void CGICreationTask::set_env(const std::string& key, const std::string& value)
{
    std::string envVar = key + "=" + value;
    _env_strings.push_back(envVar);
}

void CGICreationTask::setup_environment(
    Request& request, const Path& uri, const std::string& ip_address
)
{
    size_t length = 0;
    while (environ[length])
        ++length;

    auto last = environ + length;
    auto is_pwd = [](const std::string_view& it) { return it.rfind("PWD=", 0) == 0; };
    auto it = std::find_if(environ, last, is_pwd);

    std::string path_info = Path::canonical(uri);
    if (it != last)
    {
        std::string path_translated = std::string(&(*it)[4]) + "/" + path_info;
        set_env("PATH_TRANSLATED", path_translated);
    }
    set_env("REMOTE_ADDR", ip_address);
    set_env("PATH_INFO", path_info);
    set_env("GATEWAY_INTERFACE", "CGI/1.1");
    if (request.body().size())
        set_env("CONTENT_LENGTH", std::to_string(request.body().size()));
    if (auto value = request.headers().get(FieldName::CONTENT_TYPE))
    {
        std::string env = std::string("Content-Type: ") + (**value);
        set_env("CONTENT_TYPE", env);
    }
    set_env("SERVER_NAME", request.uri().host());
    set_env("SERVER_PORT", request.uri().port());
    set_env("SERVER_PROTOCOL", "HTTP/1.1");
    set_env("SERVER_SOFTWARE", "webserv");
    set_env("SCRIPT_NAME", request.uri().path());
    set_env("REQUEST_METHOD", request.method().to_string());
    set_env("QUERY_STRING", request.uri().query());
    set_env("PYTHONUTF8", "1");
}

CGICreationTask::CGICreationTask(
    Connection&& connection, Request& request, const Path& uri, Config& config,
    std::string cgi_executable
)
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, _pipe_fd) == -1)
    {
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }

    int pid = fork();
    if (pid == -1)
    {
        close(_pipe_fd[0]);
        close(_pipe_fd[1]);
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }
    else if (pid == 0)
    {
        close(_pipe_fd[1]);

        try
        {
            std::string relative_cd_path =
                std::string("./") + std::string(Path(uri.cbegin(), uri.cend() - 1));
            std::string script_name = std::string(Path(uri.cend() - 1, uri.cend()));

            setup_environment(request, uri, connection.ip());

            int dev_null = open("/dev/null", O_WRONLY);
            if (dev_null == -1)
                throw std::exception();
            if (dup2(_pipe_fd[0], STDIN_FILENO) == -1)
                throw std::exception();
            if (dup2(_pipe_fd[0], STDOUT_FILENO) == -1)
                throw std::exception();
            if (dup2(dev_null, STDERR_FILENO) == -1)
                throw std::exception();
            if (chdir(relative_cd_path.c_str()) == -1)
                throw std::exception();

            char* argv[] = {
                const_cast<char*>(cgi_executable.c_str()), const_cast<char*>(script_name.c_str()),
                nullptr
            };

            std::vector<const char*> envp;
            for (const auto& env : _env_strings)
            {
                envp.push_back(env.c_str());
            }
            envp.push_back(nullptr);

            (void)execve(argv[0], argv, const_cast<char**>(envp.data()));
            WARN(strerror(errno));
            close(_pipe_fd[0]);
            exit(1);
        }
        catch (std::exception& e)
        {
            close(_pipe_fd[0]);
            exit(1);
        }
    }
    close(_pipe_fd[0]);
    if (request.body().size())
    {
        WriteState write_state{
            CGIWriteTask(std::move(request), request.body(), _pipe_fd[1], pid, config),
            std::move(connection)
        };
        state(std::move(write_state));
    }
    else
    {
        ReadState read_state{
            CGIReadTask(_pipe_fd[1], config, pid),
            std::move(connection),
        };
        state(std::move(read_state));
    }
}
