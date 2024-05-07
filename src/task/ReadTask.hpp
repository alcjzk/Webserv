#pragma once

#include <vector>
#include <cstddef>
#include "Config.hpp"
#include "File.hpp"
#include "BasicTask.hpp"

class ReadTask : public BasicTask
{
    public:
        ReadTask(File&& file, size_t size, const Config& config);

        virtual void                   run() override;
        virtual std::optional<Seconds> expire_time() const override;

        std::vector<char> buffer() &&;
        bool              is_error() const;

    private:
        size_t            _size;
        std::vector<char> _buffer;
        size_t            _bytes_read_total = 0;
        bool              _is_error = false;
        Seconds           _expire_time;
};
