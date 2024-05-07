#pragma once

#include <optional>
#include "Request.hpp"
#include "BasicTask.hpp"
#include "Connection.hpp"

class ReceiveRequestTask : public BasicTask
{
    public:
        ReceiveRequestTask(Connection&& connection);

        virtual void                   run() override;
        virtual void                   abort() override;
        virtual int                    fd() const override;
        virtual std::optional<Seconds> expire_time() const override;

    private:
        /// Maximum length of the size line for chunked transfer.
        ///
        /// This limit is somewhat arbitrary, but should support most size lines.
        static const size_t CHUNKED_SIZE_LINE_MAX_LENGTH = 1024;

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
        Seconds                         _expire_time;

        /// Trims out the chunk-ext portion from `value` in place.
        static void trim_chunk_ext(std::string& value);

        /// Returns true if `value` is a valid chunk size.
        static bool is_chunk_size(const std::string& value);
};
