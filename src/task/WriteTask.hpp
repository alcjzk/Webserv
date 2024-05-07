#pragma once

#include <vector>
#include "Config.hpp"
#include "Path.hpp"
#include "BasicTask.hpp"

class WriteTask : public BasicTask
{
    public:
        /// Constructs the task.
        WriteTask(File&& file, std::vector<char>&& content, const Config& config);

        /// Constructs the task by opening `path` as the target file.
        WriteTask(const Path& path, std::vector<char>&& content, const Config& config);

        /// Performs work on the task.
        virtual void run() override;

        /// Returns the number of seconds until the task expires.
        virtual std::optional<Seconds> expire_time() const override;

        /// Returnst true if the task completed with an error.
        bool is_error() const;

    private:
        std::vector<char> _buffer;
        size_t            _bytes_written_total = 0;
        bool              _is_error = false;
        Seconds           _expire_time;
};
