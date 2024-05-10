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
        CGIReadTask(File&& read_end, const Config& config, Child&& pid);

        virtual void                   run() override;
        virtual void                   abort() override;
        virtual std::optional<Seconds> expire_time() const override;

        // Returns true if the task completed with an error.
        bool is_error() const;

        /// Returns the complete response from CGI.
        std::unique_ptr<Response> response() &&;

    private:
        enum class Expect
        {
            Headers,
            Body,
        };

        const static size_t       MAX_CONTENT = 1000000;

        Expect                    _expect = Expect::Headers;
        Reader                    _reader = Reader(4096UL);
        std::unique_ptr<Response> _response = std::make_unique<Response>(Status::OK);
        Child                     _pid;
        bool                      _is_error = false;
        Seconds                   _expire_time;

        void read_headers();
};
