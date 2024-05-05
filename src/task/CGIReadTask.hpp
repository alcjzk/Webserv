#pragma once

#include <sys/wait.h>

#include "BasicTask.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Config.hpp"

class CGIReadTask : public BasicTask
{
    public:
        virtual ~CGIReadTask() override;

        // Construct env, spawn cgi
        CGIReadTask(
            Request&& request, File&& read_end, Config& config, pid_t pid
        );

        CGIReadTask(const CGIReadTask&) = delete;
        CGIReadTask(CGIReadTask&&) = delete;

        CGIReadTask& operator=(const CGIReadTask&) = delete;
        CGIReadTask& operator=(CGIReadTask&&) = delete;

        // Read cgi output into response body, enqueue ServerSendResponseTask
        virtual void run() override;

        // TODO: override Task::abort (signal child to exit)
        void SignalhandlerChild(int sig);

        // For error state checking
        bool              is_error() const;

        // Get the buffer
        std::vector<char> buffer() &&;

    private:
        std::vector<char> _buffer;
        size_t            _size;
        size_t            _bytes_read_total = 0;
        pid_t             _pid;
        bool              _is_error = false;
};
