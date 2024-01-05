#pragma once

// TODO: Move elsewhere

#define HTTP_FIELD_DELIMETER " \t" // TODO: Replace with below
#define HTTP_LWS " \t"

#ifdef TESTS
# include <sstream>
# define TESTFAIL static_cast<std::ostringstream>((std::ostringstream() << __FILE__ << "::" << __FUNCTION__ << "()")).str()
#endif

#define HT '\t'
#define SP ' '
#define LF '\n'
#define CR '\r'
#define DEL 127

bool is_ctl(char c)
{
    if (c == DEL || c <= 31)
        return true;
    return false;
}

/// Returns true if the character is a separator, as defined by the RFC
bool is_separator(char c)
{
    const string separators("()<>@,;:\\\"/[]?={} \t");

    for (char separator : separators)
    {
        if (c == separator)
            return true;
    }
    return false;
}

/// Returns true if the input is a valid token, as defined by the RFC
bool is_token(const string& text)
{
    for (char c : text)
    {
        if (is_ctl(c) || is_separator(c))
            return false;
    }
    return true;
}
