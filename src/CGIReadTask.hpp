#pragma once

#include <sys/wait.h>

#include "Task.hpp"

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
