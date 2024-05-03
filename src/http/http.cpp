#include "http.hpp"

using std::string;

namespace http
{
    bool is_ctl(char c)
    {
        if (c == DEL || c <= 31)
            return true;
        return false;
    }

    bool is_separator(char c)
    {
        for (char separator : SEPARATORS)
        {
            if (c == separator)
                return true;
        }
        return false;
    }

    bool is_token(const string& text)
    {
        for (char c : text)
        {
            if (is_ctl(c) || is_separator(c))
                return false;
        }
        return true;
    }
} // namespace http
