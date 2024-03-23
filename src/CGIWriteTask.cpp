#include "CGIWriteTask.hpp"
#include "CGIReadTask.hpp"

// this is for preparing the content to write to the CGI
// assign _pid & _fdout

CGIWriteTask::CGIWriteTask(Request&& request, int client_fd)
: Task(client_fd, WaitFor::Writable)
, _request(request), _bytes_written(0), _client_fd(client_fd)
{
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
    {
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }

    int pid = fork();     // Fork to create a child process
    if (pid == -1)
    {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }
    else if (pid == 0) // Child process
    {
        //TODO: signla handler
        //signal(SIGINT, SignalhandlerChild());
        close(pipe_fd[0]);               // close reading
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        close(pipe_fd[1]);


        // setup env
        SetEnv("AUTH_TYPE", "basic");
        SetEnv("REDIRECT_STATUS", "200");

        const char* path = _request._request_line.request_target().c_str();
        char* argv[] = {(char*)"/usr/local/bin/python3", (char*)path, nullptr};

        // execute
        if (execve(argv[0], argv, Environment()) == -1) // argument?
        {
            throw HTTPError(Status::INTERNAL_SERVER_ERROR);
        }
        exit(0);
        }
        run();
    }

// convert std::vector<char*> to char**
char** CGIWriteTask::Environment()
{
    char** arr = new char*[_environment.size() + 1];
    size_t i = 0;

    for (std::vector<char*>::const_iterator it = _environment.begin(); it != _environment.end(); ++it) {
        arr[i++] = *it;
    }
    arr[i] = nullptr;

    return arr;
}


// setup the environment for CGI
void CGIWriteTask::SetEnv(const std::string& key, const std::string& value)
{
    std::string envVar = key + "=" + value;
    char* envPtr = strdup(envVar.c_str()); // memory leak?
    _environment.push_back(envPtr);
}

// Append QUERY_STRING=query.. into _environment
void CGIWriteTask::QueryString(const std::string& query_string)
{
    char* envPtr = strdup(query_string.c_str());
    _environment.push_back(envPtr);
}

// Write body from request to cgi, enqueue CGIReadTask
void CGIWriteTask::run()
{
    CGIReadTask(_pid, _client_fd);
}

void CGIWriteTask::SignalhandlerChild(int sig)
{
    std::cerr << "Received signal (children process): " << sig << std::endl;
    std::exit(EXIT_FAILURE);
}

