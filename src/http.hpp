#pragma once

#include <string>

/// HTTP related utilities and constants
namespace http
{
    /// Horizontal tab
    const char HT = '\t';

    /// Space
    const char SP = ' ';

    /// Linefeed
    const char LF = '\n';

    /// Carriage return
    const char CR = '\r';

    /// Delete
    const char DEL = 127;

    /// Linear whitespace
    const std::string LWS(" \t");

    /// Separators (RFC 2616 3.6)
    const std::string SEPARATORS("()<>@,;:\\\"/[]?={} \t");

    /// Returns true if the character is a control character
    bool is_ctl(char c);

    /// Returns true if the character is a separator
    bool is_separator(char c);

    /// Returns true if the input is a valid token
    bool is_token(const std::string& text);
}
