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
static void SetEnv(const std::string& key, const std::string& value, std::vector<char *>& environment)
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
static char** Environment(std::vector<char *>& environment)
{
    char** arr = new char*[environment.size() + 1];
    size_t i = 0;

    for (std::vector<char*>::const_iterator it = environment.begin(); it != environment.end();
         ++it)
    {
        arr[i++] = *it;
    }
    arr[i] = nullptr;

    return arr;
}

static void  SetupEnvironment(std::vector<char *>& environment, Request& request)
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
    Connection&& connection, Request& request, const Path& uri, Config& config
)
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, _pipe_fd) == -1)
    {
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }

    INFO("Pfd 0: " << _pipe_fd[0]);
    INFO("Pfd 1: " << _pipe_fd[1]);
    int pid = fork(); // Fork to create a child process
    // for ( auto field : request.headers())
    // {
    //     INFO("FIELD_NAME " << field.first << " FIELD_VALUE " << field.second);
    // }
    if (pid == -1)
    {
        close(_pipe_fd[0]);
        close(_pipe_fd[1]);
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }
    else if (pid == 0) // Child process
    {
        close(_pipe_fd[1]);
        // TODO: signla handler
        // signal(SIGINT, SignalhandlerChild());
        dup2(_pipe_fd[0], STDIN_FILENO); // Redirect stdout to the write end of the pipe
        dup2(_pipe_fd[0], STDOUT_FILENO); // Redirect stdout to the write end of the pipe

        // setup env
        SetupEnvironment(_environment, request);

        std::string path = uri;
        char*       argv[] = {(char*)"/usr/local/bin/python3", (char*)path.c_str(), nullptr};

        // execute
        if (execve(argv[0], argv, Environment(_environment)) == -1) // argument?
        {
            throw HTTPError(Status::INTERNAL_SERVER_ERROR);
        }
        close(_pipe_fd[0]);
        exit(0);
    }
    close(_pipe_fd[0]);
    if (request.body().size())
    {
        // const std::vector<char>& vec = request.body();
        // string str(vec.begin(), vec.end());
        // INFO("request body: " << str);
        WriteState write_state {
            CGIWriteTask(std::move(request), request.body(), _pipe_fd[1], pid, config),
            pid, std::move(connection)
        };
        state(std::move(write_state));
    }
    else
    {
        // close(_pipe_fd[1]);
        ReadState read_state{
            CGIReadTask(_pipe_fd[1], config, pid),
            std::move(connection),
        };
        state(std::move(read_state));
    }
}
