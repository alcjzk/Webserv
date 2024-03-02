#pragma once

#include <sys/wait.h>

#include "Task.hpp"
#include "Response.hpp"

class CGIReadTask : public Task
{
    public:
        virtual ~CGIReadTask() override;

        // Construct env, spawn cgi
        CGIReadTask(pid_t pid, int client_fd);

        CGIReadTask(const CGIReadTask&) = delete;
        CGIReadTask(CGIReadTask&&) = delete;

        CGIReadTask& operator=(const CGIReadTask &) = delete;
        CGIReadTask& operator=(CGIReadTask&&) = delete;

        // Read cgi output into response body, enqueue ServerSendResponseTask
        virtual void            run() override;

        // TODO: override Task::abort (signal child to exit)

    private:
        std::vector<char>  _buffer;
        Response*          _response; // CGIResponse
        size_t             _bytes_read;
        int                _client_fd;
        pid_t              _pid;
}


CGIReadTask::CGIReadTask(pid_t pid, int client_fd)
: Task(client_fd, WaitFor::Readable)
, _bytes_read(0)
, _client_fd(client_fd)
, _pid(0)
{
    //int status;
    //waitpid(pid, &status, 0);
    run();
    //close(pipe_fd[0]); // Close the read end of the pipe
    //this->body(std::move(body));
    //std::vector<char> body;
    //close(pipe_fd[1]); // Close the write end of the pipe
}

void CGIReadTask::run()
{
    //char   buffer[4096];
    //size_t bytesRead;
    //while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
    //{
    //    body.insert(body.end(), buffer, buffer + bytesRead);
    //}
}
