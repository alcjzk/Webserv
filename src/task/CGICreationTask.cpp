#include <string>
#include <string.h>
#include <utility>
#include "HTTPError.hpp"
#include "Request.hpp"
#include "Connection.hpp"
#include "CGICreationTask.hpp"
#include "Log.hpp"
#include <unistd.h>
#include <string>
#include <sys/socket.h>

using namespace cgi_creation_task;
using std::string;

// setup the environment for CGI
static void
SetEnv(const std::string& key, const std::string& value, std::vector<char*>& environment)
{
    std::string envVar = key + "=" + value;
    char*       envPtr = strdup(envVar.c_str()); // memory leak?
    environment.push_back(envPtr);
}

// Append QUERY_STRING=query.. into _environment
// static void QueryString(const std::string& query_string, std::vector<char *>& environment)
// {
//     char* envPtr = strdup(query_string.c_str());
//     environment.push_back(envPtr);
// }

// static void SignalhandlerChild(int sig)
// {
//     std::cerr << "Received signal (children process): " << sig << std::endl;
//     std::exit(EXIT_FAILURE);
// }

// convert std::vector<char*> to char**
static char** Environment(std::vector<char*>& environment)
{
    char** arr = new char*[environment.size() + 1];
    size_t i = 0;

    for (std::vector<char*>::const_iterator it = environment.begin(); it != environment.end(); ++it)
    {
        arr[i++] = *it;
    }
    arr[i] = nullptr;

    return arr;
}

static void SetupEnvironment(std::vector<char*>& environment, Request& request)
{
    SetEnv("AUTH_TYPE", "basic", environment);
    SetEnv("REDIRECT_STATUS", "200", environment);
    SetEnv("GATEWAY_INTERFACE", "CGI/1.1", environment);
    if (request.body().size())
        SetEnv("CONTENT_LENGTH", std::to_string(request.body().size()), environment);
    SetEnv("SERVER_NAME", request.uri().host(), environment);
    SetEnv("SERVER_PORT", request.uri().port(), environment);
    SetEnv("SERVER_PROTOCOL", "HTTP/1.1", environment);
    SetEnv("SERVER_SOFTWARE", "webserv", environment);
    SetEnv("SCRIPT_NAME", request.uri().path(), environment);
    SetEnv("REQUEST_METHOD", request.method().to_string(), environment);
    SetEnv("QUERY_STRING", request.uri().query(), environment);
    // QueryString(request.uri().query(), environment);
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

    INFO("Pfd 0: " << _pipe_fd[0]);
    INFO("Pfd 1: " << _pipe_fd[1]);
    INFO("Request target " << request.request_line().request_target().c_str());
    INFO("Path URI " << uri);
    INFO("Path URI canonical " << Path::canonical(uri));
    INFO("New path without end " << Path(uri.cbegin(), uri.cend() - 1))
    // for ( auto field : request.headers())
    // {
    //     INFO("FIELD_NAME " << field.first << " FIELD_VALUE " << field.second);
    // }
    int pid = fork(); // Fork to create a child process
    if (pid == -1)
    {
        close(_pipe_fd[0]);
        close(_pipe_fd[1]);
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }
    else if (pid == 0)
    {
        // chdir(request.request_line().request_target().c_str());
        close(_pipe_fd[1]);
        int dev_null = open("/dev/null", O_WRONLY);

        if (dev_null == -1)
        {
            INFO("dev null fail: " << strerror(errno));
            close(_pipe_fd[0]);
            exit(1);
        }
        if (dup2(_pipe_fd[0], STDIN_FILENO) == -1)
        {
            close(_pipe_fd[0]);
            exit(1);
        }
        if (dup2(_pipe_fd[0], STDOUT_FILENO) == -1)
        {
            close(_pipe_fd[0]);
            exit(1);
        }
        if (dup2(dev_null, STDERR_FILENO) == -1)
        {
            close(_pipe_fd[0]);
            exit(1);
        }
        SetupEnvironment(_environment, request);

        std::string path = uri;
        char*       argv[] = {
            const_cast<char*>(cgi_executable.c_str()), const_cast<char*>(path.c_str()), nullptr
        };

        // execute
        (void)execve(argv[0], argv, Environment(_environment)); // argument?
        close(_pipe_fd[0]);
        exit(1);
    }
    INFO("Child's pid is " << pid);
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
        // close(_pipe_fd[1]);
        INFO("yeet");
        ReadState read_state{
            CGIReadTask(_pipe_fd[1], config, pid),
            std::move(connection),
        };
        state(std::move(read_state));
    }
}
