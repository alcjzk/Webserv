#pragma once

#include <cstddef>
#include <optional>
#include "Server.hpp"
#include "Reader.hpp"
#include "Request.hpp"
#include "BasicTask.hpp"
#include "File.hpp"

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
        // TODO: void receive_body()

        // Util
        void   fill_buffer();
        char*  buffer_head();
        size_t buffer_size_available();
        void   disable_linger();

        // TODO: Use value from config + expanding buffersize?
        static const size_t             _header_buffer_size = 4096;
        Expect                          _expect = Expect::RequestLine;
        size_t                          _bytes_received_total = 0;
        Reader                          _reader;
        std::optional<Request::Builder> _builder = Request::Builder();
        bool                            _is_partial_data = true;
        const Server&                   _server;
};
