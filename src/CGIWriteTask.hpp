#pragma once

#include <sys/wait.h>
#include "Task.hpp"

class CGIWriteTask : public Task
{
    public:
        virtual ~CGIWriteTask() override;

        // Construct env, spawn cgi
        CGIWriteTask(Request&& request, int client_fd);

        CGIWriteTask(const CGIWriteTask&) = delete;
        CGIWriteTask(CGIWriteTask&&) = delete;

        CGIWriteTask& operator=(const CGIWriteTask &) = delete;
        CGIWriteTask& operator=(CGIWriteTask&&) = delete;

        // Write body from request to cgi, enqueue CGIReadTask
        virtual void            run() override;

        // TODO: override Task::abort (signal child to exit)

    private:
        void set_env(const std::string& key, const std::string& value);

        Request            _request;
        std::vector<char*> _environment;
        size_t             _bytes_written;
        pid_t              _pid;
        int                _client_fd;
        int                _fd_out; // stdout of child, saved for later use
};
