#pragma once

#include <cstddef>
#include <vector>
#include "Status.hpp"
#include "Header.hpp"
#include "ContentLength.hpp"

class Response
{
    public:
        virtual ~Response() = default;

        Response(Status = Status::OK);
        Response(Response&&) = default;
        Response(const Response&) = default;

        Response& operator=(Response&& other) = default;
        Response& operator=(const Response& other) = default;

        /// Sends the response using SEND(3).
        ///
        /// @throw std::runtime_error
        /// @return true if the message was fully sent, false otherwise.
        bool send(int fd);

        void header(const Header& header);
        void header(Header&& header);

        const std::vector<Header>& headers() const;

        void body(std::vector<char>&& body);
        void body(const std::vector<char>& body);

        const std::vector<char>& body() const;

        void content_length(ContentLength content_length);

        bool _keep_alive = true;

    private:
        Status              _status;
        std::vector<char>   _buffer;
        size_t              _size;
        size_t              _size_remaining;
        bool                _is_built;
        std::vector<Header> _headers;
        std::vector<char>   _body;

        void build();
};
