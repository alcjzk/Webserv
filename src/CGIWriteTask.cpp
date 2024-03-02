#include "CGIWriteTask.hpp"

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

        char* path = static_cast<char*>(_request._request_line);
        char* argv[] = {(char*)"/usr/local/bin/python3", _request._request_line.c_str(), nullptr};

        // convert std::vector<char*> to char**
        std::unique_ptr<char*[]> vectorToCharPtrArray(const std::vector<char*>& vec) {
        std::unique_ptr<char*[]> arr(new char*[vec.size() + 1]);

        size_t i = 0;
        for (std::vector<char*>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
            arr[i++] = *it;
        }
        arr[i] = nullptr;

        // execute
        if (execve(argv[0], argv, vectorToCharPtrArray) == -1) // argument?
        {
            throw HTTPError(Status::INTERNAL_SERVER_ERROR);
        }
        exit(0);
        }
    }
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

}

void CGIWriteTask::SignalhandlerChild(int sig)
{
    std::cerr << "Received signal (children process): " << sig << std::endl;
    std::exit(EXIT_FAILURE);
}

