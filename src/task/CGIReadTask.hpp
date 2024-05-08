#pragma once

#include <sys/wait.h>

#include "BasicTask.hpp"
#include "Config.hpp"

class CGIReadTask : public BasicTask
{
    public:
        virtual ~CGIReadTask() override;

        // Construct env, spawn cgi
        CGIReadTask(int read_end, const Config& config, pid_t pid);

        CGIReadTask(const CGIReadTask&) = delete;
        CGIReadTask(CGIReadTask&&);

        CGIReadTask& operator=(const CGIReadTask&) = delete;
        CGIReadTask& operator=(CGIReadTask&&);

        // Read cgi output into response body, enqueue ServerSendResponseTask
        virtual void run() override;

        // TODO: override Task::abort (signal child to exit)
        void SignalhandlerChild(int sig);

        // For error state checking
        bool is_error() const;

        // Get the buffer
        std::vector<char>&& buffer();

        // Abort override
        void abort() override;

        std::optional<Seconds> expire_time() const override;

    private:
        std::vector<char>    _buffer;
        std::optional<pid_t> _pid;
        bool                 _is_error = false;
        int                  _exit_status = 0;
        Seconds              _expire_time;
};
