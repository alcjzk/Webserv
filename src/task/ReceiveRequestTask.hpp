#pragma once

#include <cstddef>
#include <optional>
#include "Task.hpp"
#include "Server.hpp"
#include "Reader.hpp"
#include "Request.hpp"
#include "File.hpp"

class ReceiveRequestTask : public Task
{
    public:
        ReceiveRequestTask(const Server& server, File&& file);

        virtual void run() override;
        virtual void abort() override;

    private:
        typedef enum Expect
        {
            REQUEST_LINE,
            HEADERS
        } Expect;

        // State impl
        void                            receive_start_line();
        void                            receive_headers();
        // TODO: void receive_body()

        // Util
        void                            fill_buffer();
        char*                           buffer_head();
        size_t                          buffer_size_available();

        // TODO: Use value from config + expanding buffersize?
        static const size_t             _header_buffer_size = 4096;
        Expect                          _expect;
        size_t                          _bytes_received_total;
        Reader                          _reader;
        std::optional<Request::Builder> _builder = Request::Builder();
        bool                            _is_partial_data;
        const Server&                   _server;
};
