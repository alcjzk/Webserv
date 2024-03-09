#pragma once

#include <string>
#include <string_view>

/// https://datatracker.ietf.org/doc/html/rfc9110#name-http-uri-scheme
class HttpUri
{
    public:
        static constexpr std::string_view SCHEME = "http";
        static constexpr std::string_view HIER_PART_PREFIX = "//";
        static constexpr std::string_view PORT_DEFAULT = "80";
        static constexpr std::string_view PREFIX = "http://";
        /// Minimum length for the hier-part portion of the scheme.
        static constexpr size_t           HIER_PART_LENGTH_MIN();

        /// Minimum length for the authority portion of the scheme.
        static const size_t               AUTHORITY_LENGTH_MIN = 1;

        /// Minimum length for the path portion of the scheme.
        static const size_t               PATH_LENGTH_MIN = 1;

        HttpUri() = default;

        HttpUri(const std::string& request_target, const std::string& host);

        const std::string& host() const;
        const std::string& port() const;
        const std::string& path() const;
        const std::string& query() const;

    private:
        std::string                     _host;
        std::string                     _port;
        std::string                     _path;
        std::string                     _query;

        template <typename String> void authority(const String& authority)
        {
            size_t host_length;

            host_length = std::min(authority.find_first_of(':'), authority.length());
            _host = std::string(authority.cbegin(), authority.cbegin() + host_length);

            if (host_length == authority.length())
                _port = PORT_DEFAULT;
            else
                _port = std::string(authority.cbegin() + host_length + 1, authority.cend());
        }
};

#ifdef TEST

class HttpUriTest : private HttpUri
{
    public:
        static void absolute_form_test();
        static void origin_form_test();
};

#endif
