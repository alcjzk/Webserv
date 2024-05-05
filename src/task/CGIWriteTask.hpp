#pragma once

#include <sys/wait.h>
#include <unistd.h>
#include "File.hpp"
#include "BasicTask.hpp"
#include "Request.hpp"

class CGIWriteTask : public BasicTask
{
    public:
        virtual ~CGIWriteTask() override;

        // Construct env, spawn cgi
        CGIWriteTask(
            Request&& request, const Request::Body& post_body, File&& write_end, pid_t pid, Config& config, int read_end
        );

        CGIWriteTask(const CGIWriteTask&) = delete;
        CGIWriteTask(CGIWriteTask&&) = delete;

        CGIWriteTask& operator=(const CGIWriteTask&) = delete;
        CGIWriteTask& operator=(CGIWriteTask&&) = delete;

        // Write body from request to cgi, enqueue CGIReadTask
        virtual void run() override;

        /// Returnst true if the task completed with an error.
        bool is_error() const;

        // TODO: override Task::abort (signal child to exit)
        void SignalhandlerChild(int sig);

        // Getters
        int     read_end() const;
        Config& config()   const;

    private:
        Config&            _config;
        Request            _request;
        int                _read_end;
        std::vector<char>  _post_body;
        std::vector<char*> _environment;
        size_t             _bytes_written_total = 0;
        pid_t              _pid;
        bool               _is_error = false;
};
