#pragma once

#include <optional>
#include "Request.hpp"
#include "BasicTask.hpp"
#include "Connection.hpp"
#include "ContentLength.hpp"

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
            Body,
        };

        // State impl
        void receive_start_line();
        void receive_headers();
        void receive_body();

        // Util
        void fill_buffer();
        void disable_linger();

        Connection                      _connection;
        Expect                          _expect = Expect::RequestLine;
        ContentLength                   _content_length = 0;
        std::optional<Request::Builder> _builder = Request::Builder();
        bool                            _is_partial_data = false;
};
