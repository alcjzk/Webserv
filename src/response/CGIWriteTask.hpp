#pragma once

#include <sys/wait.h>
#include <unistd.h>
#include <exception>
#include "HTTPError.hpp"
#include "Task.hpp"
#include "Request.hpp"

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

        char** Environment();

        // TODO: override Task::abort (signal child to exit)

    private:
        void SetEnv(const std::string& key, const std::string& value);
        void QueryString(const std::string& query_string);
        void SignalhandlerChild(int sig);

        Request            _request;
        std::vector<char*> _environment;
        size_t             _bytes_written;
        pid_t              _pid;
        int                _client_fd;
        int                _fd_out; // stdout of child, saved for later use
};
