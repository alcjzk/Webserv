#include <string>
#include <string.h>
#include <utility>
#include "HTTPError.hpp"
#include "Request.hpp"
#include "Connection.hpp"
#include "CGICreationTask.hpp"

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
    Connection&& connection, Request& request, const Path& uri, Config& config
)
{
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
    {
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }

    int pid = fork(); // Fork to create a child process
    if (pid == -1)
    {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }
    else if (pid == 0) // Child process
    {
        // TODO: signla handler
        // signal(SIGINT, SignalhandlerChild());
        close(pipe_fd[0]);               // close reading
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        close(pipe_fd[1]);

        // setup env
        SetEnv("AUTH_TYPE", "basic", _environment);
        SetEnv("REDIRECT_STATUS", "200", _environment);

        const char* path = request.request_line().request_target().c_str();
        char*       argv[] = {(char*)"/usr/local/bin/python3", (char*)path, nullptr};

        // execute
        if (execve(argv[0], argv, Environment(_environment)) == -1) // argument?
        {
            throw HTTPError(Status::INTERNAL_SERVER_ERROR);
        }
        exit(0);
    }

    WriteState write_state {
        CGIWriteTask(std::move(request), request.body(), File(pipe_fd[0]), pid, config,  pipe_fd[1]),
        pid, std::move(connection)
    };

    state(std::move(write_state));
}
