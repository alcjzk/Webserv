#pragma once

#include <cctype>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <string>
#include <utility>
#include "FieldName.hpp"
#include "FieldValue.hpp"
#include "HTTPVersion.hpp"

/// HTTP related utilities and constants
namespace http
{
    const HTTPVersion VERSION(1, 1);

    /// Horizontal tab (RFC 9110)
    const char HTAB = '\t';

    /// Space (RFC 9110)
    const char SP = ' ';

    /// Linefeed
    const char LF = '\n';

    /// Carriage return
    const char CR = '\r';

    /// Delete
    const char DEL = 127;

    /// Double quote
    const char DQUOTE = '"';

    /// Linear whitespace
    const std::string LWS(" \t");

    const char* const CRLF = "\r\n";

    /// Returns true if the input is a valid token
    bool is_token(const std::string& text);

    /// Returns true if `c` is a `tchar` (RFC 9110).
    bool is_tchar(unsigned char c);

    /// Returns true if `c` is whitespace (RFC 9110).
    bool is_whitespace(unsigned char c);

    /// Returns true if `c` is a field-vchar (RFC 9110).
    bool is_field_vchar(unsigned char c);

    /// Returns true if `c` is qdtext (RFC 9110).
    bool is_qdtext(unsigned char c);

    /// Returns true if `c` is obs-text (RFC 9110).
    bool is_obs_text(unsigned char c);

    /// Returns true if `value` is an absolute path (RFC 9110).
    bool is_absolute_path(const std::string& value);

    /// Returns true if `value` is a valid query (RFC 3986).
    bool is_query(const std::string& value);

    template <typename ForwardIt>
    std::string pct_decode(ForwardIt first, ForwardIt last);

    std::pair<FieldName, FieldValue> parse_field(const std::string& field);
} // namespace http

template <typename ForwardIt>
std::string http::pct_decode(ForwardIt head, ForwardIt last)
{
    std::string result;
    result.reserve(std::distance(head, last));

    auto is_hex = [](unsigned char c) { return std::isxdigit(c); };

    while (head != last)
    {
        if (*head == '%')
        {
            head = std::next(head);
            if (std::distance(head, last) < 2)
                throw std::runtime_error("invalid pct encoding");

            auto end = std::next(head);
            if (!std::all_of(head, end, is_hex))
                throw std::runtime_error("invalid pct encoding");

            unsigned char value = std::stoul(std::string(head, std::next(end)), nullptr, 16);
            result.push_back(value);
            head = std::next(end);
        }
        else
        {
            result.push_back(*head);
            head = std::next(head);
        }
    }
    return result;
}
