#pragma once

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>
#include <ostream>
#include "Path.hpp"

/// https://datatracker.ietf.org/doc/html/rfc9110#name-http-uri-scheme
class HttpUri
{
    public:
        static constexpr std::string_view PORT_DEFAULT = "80";
        static constexpr std::string_view PREFIX = "http://";

        HttpUri(const std::string& request_target, const std::string& host);

        const std::string& host() const;
        const std::string& port() const;
        const Path&        path() const;
        const std::string& query() const;

    private:
        std::string _host;
        std::string _port;
        Path        _path;
        std::string _query;

        template <typename String>
        void authority(const String& authority)
        {
            size_t host_length;

            host_length = std::min(authority.find_first_of(':'), authority.length());
            _host.reserve(host_length);
            auto to_lowercase = [](unsigned char c) { return std::tolower(c); };
            (void)std::transform(
                authority.begin(), authority.begin() + host_length, std::back_inserter(_host),
                to_lowercase
            );

            if (host_length == authority.length())
                _port = PORT_DEFAULT;
            else
                _port = std::string(authority.cbegin() + host_length + 1, authority.cend());
        }
};

std::ostream& operator<<(std::ostream& os, const HttpUri& http_uri);

#ifdef TEST

class HttpUriTest : private HttpUri
{
    public:
        static void absolute_form_test();
        static void origin_form_test();
        static void absolute_form_ignores_host_header_test();
        static void pct_decoded_test();
        static void host_case_insensitive_test();
        static void scheme_case_insensitive_test();
};

#endif
