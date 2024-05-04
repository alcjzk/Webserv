#pragma once

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

    std::pair<FieldName, FieldValue> parse_field(const std::string& field);
} // namespace http
