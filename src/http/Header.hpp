#pragma once

#include <string>
#include <ostream>

class Header
{
    public:
        /// Constructs the header from a name value pair.
        ///
        /// @warning
        /// This function does not validate the parameters in any way.
        ///
        /// @param name the header name
        /// @param value the header value
        Header(std::string name, std::string value);

        /// Constructs the header from a text value.
        ///
        /// Any LWS leading the header name, as well as LWS leading and
        /// trailing name is removed. LWS within name is not modified.
        ///
        /// @param text the input text
        /// @throw `HTTPError` (Bad request) - if the input cannot be converted
        /// into a valid header
        Header(const std::string& text);

        /// Appends field-content to the header value.
        ///
        /// Leading LWS is replaced with a single SP and trailing LWS is
        /// removed.
        ///
        /// @param value field-content to append
        void append(const std::string& value);

        std::string _name;
        std::string _value;
};

std::ostream& operator<<(std::ostream& os, const Header& header);

#ifdef TEST

class HeaderTest : public Header
{
    public:
        static void text_construct_case_insensitive_test();
};

#endif
