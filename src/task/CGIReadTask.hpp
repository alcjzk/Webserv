#pragma once

#include <cstddef>
#include <sys/wait.h>
#include <memory>

#include "BasicTask.hpp"
#include "Child.hpp"
#include "Config.hpp"
#include "Reader.hpp"
#include "Response.hpp"

class CGIReadTask : public BasicTask
{
    public:
        enum class Expect
        {
            Headers,
            Body,
        };

        virtual ~CGIReadTask() override;

        // Construct env, spawn cgi
        CGIReadTask(int read_end, const Config& config, Child&& pid);

        CGIReadTask(const CGIReadTask&) = delete;
        CGIReadTask(CGIReadTask&&) = default;

        CGIReadTask& operator=(const CGIReadTask&) = delete;
        CGIReadTask& operator=(CGIReadTask&&) = default;

        // Read cgi output into response body, enqueue ServerSendResponseTask
        virtual void run() override;

        // For error state checking
        bool is_error() const;

        /// Returns the complete response from CGI.
        std::unique_ptr<Response> response() &&;

        // Abort override
        void abort() override;

        std::optional<Seconds> expire_time() const override;

    private:
        Expect                    _expect = Expect::Headers;
        Reader                    _reader = Reader(4096UL);
        std::unique_ptr<Response> _response = std::make_unique<Response>(Status::OK);
        Child                     _pid;
        bool                      _is_error = false;
        const static size_t       _upload_limit = 1000000;
        Seconds                   _expire_time;

        void read_headers();
};
