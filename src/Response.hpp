#pragma once

#include <cstddef>

/// An abstract Response that can be sent.
///
/// The response class provides the member function send(), using the derived
/// definitions for buffer_head(), buffer_bytes_left() and buffer_advance().
class Response
{
    public:
        virtual ~Response() = default;

        /// Sends the response using SEND(3).
        ///
        /// @throw std::runtime_error
        /// @return true if the message was fully sent, false otherwise.
        bool send(int fd);

    protected:
        /// Returns a pointer to the current position in the send buffer.
        virtual const void* buffer_head() const throw() = 0;

        /// Returns the amount of bytes remaining in the send buffer.
        virtual size_t      buffer_bytes_left() const throw() = 0;

        /// Advances the current position in the send buffer by the given amount.
        ///
        /// @param bytes_count Amount of bytes to advance the buffer by.
        virtual void        buffer_advance(size_t bytes_count) throw() = 0;
};

#include <string>

/// An example definition of a derived response to respond with an arbitraty
/// string.
class TextResponse : public Response
{
    public:
        TextResponse(std::string text);

    protected:
        virtual const void* buffer_head() const throw() override;
        virtual size_t      buffer_bytes_left() const throw() override;
        virtual void        buffer_advance(size_t bytes_count) throw() override;

    private:
        std::string _text;
        size_t      _bytes_left;
};
