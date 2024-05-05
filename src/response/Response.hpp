#pragma once

#include <cstddef>
#include <vector>
#include "Status.hpp"
#include "ContentLength.hpp"
#include "FieldName.hpp"
#include "FieldValue.hpp"
#include "FieldMap.hpp"

class Response
{
    public:
        using Header = std::pair<FieldName, FieldValue>;

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

        const FieldMap& headers() const;
        FieldMap&       headers();

        void body(std::vector<char>&& body);
        void body(const std::vector<char>& body);
        void body(const std::string& body);

        const std::vector<char>& body() const;

        Status status() const;

        bool          keep_alive = true;
        ContentLength content_length = 0;

    private:
        Status            _status;
        std::vector<char> _buffer;
        size_t            _size;
        size_t            _size_remaining;
        bool              _is_built;
        FieldMap          _headers;
        std::vector<char> _body;

        void build();
};
