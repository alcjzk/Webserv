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


CGICreationTask::CGICreationTask(
    Connection&& connection, Request& request, const Path& uri, const Config& config
)
{
    if (pipe(_pipe_fd) == -1)
    {
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }

    INFO("Pfd 0: " << _pipe_fd[0]);
    INFO("Pfd 1: " << _pipe_fd[1]);
    int pid = fork(); // Fork to create a child process
    for ( auto field : request.headers())
    {
        INFO("FIELD_NAME " << field.first << " FIELD_VALUE " << field.second);
    }
    if (pid == -1)
    {
        close(_pipe_fd[0]);
        close(_pipe_fd[1]);
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }
    else if (pid == 0) // Child process
    {
        // TODO: signla handler
        // signal(SIGINT, SignalhandlerChild());
        close(_pipe_fd[0]);               // close reading
        dup2(_pipe_fd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        close(_pipe_fd[1]);

        // setup env
        SetEnv("AUTH_TYPE", "basic", _environment);
        SetEnv("REDIRECT_STATUS", "200", _environment);
        SetEnv("GATEWAY_INTERFACE", "CGI/1.1", _environment);
        if (request.body().size())
            SetEnv("CONTENT_LENGTH", std::to_string(request.body().size()), _environment);
        SetEnv("SERVER_NAME", request.uri().host(), _environment);
        SetEnv("SERVER_PORT", request.uri().port(), _environment);
        SetEnv("SERVER_SOFTWARE", "webserv", _environment);

        std::string path = uri;
        char*       argv[] = {(char*)"/usr/local/bin/python3", (char*)path.c_str(), nullptr};

        // execute
        if (execve(argv[0], argv, Environment(_environment)) == -1) // argument?
        {
            throw HTTPError(Status::INTERNAL_SERVER_ERROR);
        }
        exit(0);
    }

    if (request.body().size())
    {
        WriteState write_state {
            CGIWriteTask(std::move(request), request.body(), File(_pipe_fd[0]), pid, config,  _pipe_fd[1]),
            pid, std::move(connection)
        };
        state(std::move(write_state));
    }
    else
    {
        close(_pipe_fd[1]);
        ReadState read_state{
            CGIReadTask(_pipe_fd[0], config, pid),
            std::move(connection),
        };
        state(std::move(read_state));
    }
}
