#pragma once

#include <sys/wait.h>
#include <unistd.h>
#include "BasicTask.hpp"
#include "Request.hpp"

class CGIWriteTask : public BasicTask
{
    public:
        virtual ~CGIWriteTask() override;

        // Construct env, spawn cgi
        CGIWriteTask(
            Request&& request, const Request::Body& post_body, int write_end, pid_t pid,
            Config& config
        );

        CGIWriteTask(const CGIWriteTask&) = delete;
        CGIWriteTask(CGIWriteTask&&);

        CGIWriteTask& operator=(const CGIWriteTask&) = delete;
        CGIWriteTask& operator=(CGIWriteTask&&);

        void abort() override;

        // Write body from request to cgi, enqueue CGIReadTask
        virtual void run() override;

        // Making sure, that the fd returned to the poll function is _write_end
        virtual int fd() const override;

        /// Returnst true if the task completed with an error.
        bool is_error() const;

        // TODO: override Task::abort (signal child to exit)
        void SignalhandlerChild(int sig);

        // Getters
        int                    write_end() const;
        const Config&          config() const;
        std::optional<Seconds> expire_time() const override;

    private:
        Config               _config;
        Request              _request;
        int                  _write_end;
        std::vector<char>    _post_body;
        std::vector<char*>   _environment;
        size_t               _bytes_written_total = 0;
        std::optional<pid_t> _pid;
        bool                 _is_error = false;
        int                  _exit_status = 0;
        Seconds              _expire_time;
};
