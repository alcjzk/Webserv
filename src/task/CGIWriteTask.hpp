#pragma once

#include <sys/wait.h>
#include <unistd.h>
#include "BasicTask.hpp"
#include "Request.hpp"
#include "Child.hpp"

class CGIWriteTask : public BasicTask
{
    public:
        CGIWriteTask(
            Request&& request, const Request::Body& post_body, File&& write_end, Child&& pid,
            const Config& config
        );

        virtual void                   run() override;
        virtual void                   abort() override;
        virtual std::optional<Seconds> expire_time() const override;

        /// Returns true if the task completed with an error.
        int error() const;

        const Config& config() const;

        Child take_pid() &&;
        File  take_fd() &&;

    private:
        const Config&     _config;
        Request           _request;
        std::vector<char> _post_body;
        size_t            _bytes_written_total = 0;
        Child             _pid;
        int               _error_status = 0;
        Seconds           _expire_time;
};
