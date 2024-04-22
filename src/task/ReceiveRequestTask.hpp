#pragma once

#include <optional>
#include "Request.hpp"
#include "BasicTask.hpp"
#include "Connection.hpp"

class ReceiveRequestTask : public BasicTask
{
    public:
        ReceiveRequestTask(Connection&& connection);

        virtual void run() override;
        virtual void abort() override;
        virtual int  fd() const override;

    private:
        enum class Expect
        {
            RequestLine,
            Headers,
            ChunkSize,
            LastChunk,
            Chunk,
            Body,
        };

        // State impl
        void receive_start_line();
        void receive_headers();
        void receive_chunk_size();
        void receive_chunk();
        void receive_last_chunk();
        void receive_body();
        void realign_reader();

        // Util
        void fill_buffer();
        void disable_linger();

        Connection                      _connection;
        Expect                          _expect = Expect::RequestLine;
        std::optional<Request::Builder> _builder = Request::Builder();
        size_t                          _chunk_size = 0;
        size_t                          _chunked_position = 0;
        std::vector<char>               _chunked_body;
        bool                            _is_partial_data = false;
};
