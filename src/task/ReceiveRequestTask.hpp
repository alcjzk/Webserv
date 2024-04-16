#pragma once

#include <cstddef>
#include <optional>
#include "Server.hpp"
#include "Reader.hpp"
#include "Request.hpp"
#include "BasicTask.hpp"
#include "File.hpp"
#include "ContentLength.hpp"

class ReceiveRequestTask : public BasicTask
{
    public:
        ReceiveRequestTask(const Server& server, File&& file);

        virtual void run() override;
        virtual void abort() override;

    private:
        enum class Expect
        {
            RequestLine,
            Headers,
            Body,
        };

        // State impl
        void receive_start_line();
        void receive_headers();
        void receive_body();

        // Util
        void fill_buffer();
        void disable_linger();

        // TODO: Use value from config + expanding buffersize?
        static const size_t _header_buffer_size = 4096;

        Expect                          _expect = Expect::RequestLine;
        Reader                          _reader;
        ContentLength                   _content_length = 0;
        std::optional<Request::Builder> _builder = Request::Builder();
        bool                            _is_partial_data = true;
        const Server&                   _server;
};
