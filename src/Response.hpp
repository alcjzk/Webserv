#pragma once

#include <cstddef>
#include <vector>
#include "Status.hpp"
#include "Header.hpp"

class Response
{
    public:
        enum class Connection
        {
            KeepAlive,
            Close,
        };

        virtual ~Response();

        Response(Status = Status::OK, Connection connection = Connection::KeepAlive);
        Response(Response&&) = delete;
        Response(const Response&) = delete;

        Response&                  operator=(Response&& other) = delete;
        Response&                  operator=(const Response& other) = delete;

        /// Sends the response using SEND(3).
        ///
        /// @throw std::runtime_error
        /// @return true if the message was fully sent, false otherwise.
        bool                       send(int fd);

        void                       header(const Header& header);
        void                       header(Header&& header);

        const std::vector<Header>& headers() const;

        void                       body(std::vector<char>&& body);
        void                       body(const std::vector<char>& body);

        const std::vector<char>&   body() const;

        void                       content_length(size_t content_length);

        Connection                 _connection;

    private:
        Status              _status;
        char*               _buffer;
        size_t              _size;
        size_t              _size_remaining;
        bool                _is_built;
        std::vector<Header> _headers;
        std::vector<char>   _body;

        void                build();
};
