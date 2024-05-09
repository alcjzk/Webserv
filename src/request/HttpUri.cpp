#include <algorithm>
#include <stdexcept>
#include "HttpUri.hpp"
#include "HTTPError.hpp"
#include "http.hpp"

using std::string;

HttpUri::HttpUri(const std::string& request_target, const std::string& host)
{
    if (request_target.empty())
        throw std::runtime_error("request target cannot be empty");

    size_t query_offset = std::min(request_target.find_first_of('?'), request_target.length());
    if (query_offset != request_target.length())
    {
        string query = request_target.substr(query_offset + 1);
        if (!http::is_query(query))
            throw HTTPError(Status::BAD_REQUEST);
        query = http::pct_decode(query.begin(), query.end());
        _query = std::move(query);
    }

    if (request_target[0] == '/')
    {
        string path = request_target.substr(0, query_offset);
        if (!http::is_absolute_path(path))
            throw HTTPError(Status::BAD_REQUEST);
        path = http::pct_decode(path.begin(), path.end());
        _path = Path::canonical(path);
        authority(host);
    }
    else
    {
        string prefix = request_target.substr(0, PREFIX.length());
        auto   to_lowercase = [](unsigned char c) { return std::tolower(c); };
        (void)std::transform(prefix.begin(), prefix.end(), prefix.begin(), to_lowercase);
        if (prefix != PREFIX)
            throw std::runtime_error("invalid scheme prefix");

        if (PREFIX.length() == request_target.length())
            throw std::runtime_error("expected host");

        size_t path_offset =
            std::min(request_target.find_first_of("/", PREFIX.length()), query_offset);
        if (path_offset < query_offset)
        {
            string path = request_target.substr(path_offset, query_offset - path_offset);
            if (!http::is_absolute_path(path))
                throw HTTPError(Status::BAD_REQUEST);
            path = http::pct_decode(path.begin(), path.end());
            _path = Path::canonical(path);
        }
        else
            _path = "/";

        authority(std::string_view(
            request_target.c_str() + PREFIX.length(), path_offset - PREFIX.length()
        ));
    }
}

const Path& HttpUri::path() const
{
    return _path;
}

const std::string& HttpUri::query() const
{
    return _query;
}

const std::string& HttpUri::host() const
{
    return _host;
}

const std::string& HttpUri::port() const
{
    return _port;
}

std::ostream& operator<<(std::ostream& os, const HttpUri& http_uri)
{
    os << http_uri.PREFIX;
    os << http_uri.host() << ':' << http_uri.port();
    os << http_uri.path();
    if (!http_uri.query().empty())
        os << '?' << http_uri.query();
    return os;
}

#ifdef TEST

#include "testutils.hpp"

void HttpUriTest::absolute_form_test()
{
    BEGIN

    HttpUri uri1("http://example.com:80/path?key=value", "example.com:80");
    EXPECT(uri1.host() == "example.com");
    EXPECT(uri1.port() == "80");
    EXPECT(uri1.path() == "/path");
    EXPECT(uri1.query() == "key=value");

    // No query
    HttpUri uri2("http://example.com:80/path", "example.com:80");
    EXPECT(uri2.host() == "example.com");
    EXPECT(uri2.port() == "80");
    EXPECT(uri2.path() == "/path");
    EXPECT(uri2.query() == "");

    // No path
    HttpUri uri3("http://example.com:80?key=value", "example.com:80");
    EXPECT(uri3.host() == "example.com");
    EXPECT(uri3.port() == "80");
    EXPECT(uri3.path() == "/");
    EXPECT(uri3.query() == "key=value");

    // No port
    HttpUri uri4("http://example.com/path?key=value", "example.com");
    EXPECT(uri4.host() == "example.com");
    EXPECT(uri4.port() == "80");
    EXPECT(uri4.path() == "/path");
    EXPECT(uri4.query() == "key=value");

    // No path or query
    HttpUri uri5("http://example.com", "example.com");
    EXPECT(uri5.host() == "example.com");
    EXPECT(uri5.port() == "80");
    EXPECT(uri5.path() == "/");
    EXPECT(uri5.query() == "");

    END
}

void HttpUriTest::origin_form_test()
{
    BEGIN

    // Root
    HttpUri uri1("/", "example.com");
    EXPECT(uri1.host() == "example.com");
    EXPECT(uri1.port() == "80");
    EXPECT(uri1.path() == "/");
    EXPECT(uri1.query() == "");

    // Root with query
    HttpUri uri2("/?query", "example.com");
    EXPECT(uri2.host() == "example.com");
    EXPECT(uri2.port() == "80");
    EXPECT(uri2.path() == "/");
    EXPECT(uri2.query() == "query");

    // Path
    HttpUri uri3("/path", "example.com");
    EXPECT(uri3.host() == "example.com");
    EXPECT(uri3.port() == "80");
    EXPECT(uri3.path() == "/path");
    EXPECT(uri3.query() == "");

    // Path with query
    HttpUri uri4("/path?query", "example.com");
    EXPECT(uri4.host() == "example.com");
    EXPECT(uri4.port() == "80");
    EXPECT(uri4.path() == "/path");
    EXPECT(uri4.query() == "query");

    END
}

void HttpUriTest::absolute_form_ignores_host_header_test()
{
    BEGIN

    HttpUri uri("http://example.com/", "notreal.com");
    EXPECT(uri.host() == "example.com");

    END
}

void HttpUriTest::pct_decoded_test()
{
    BEGIN

    {
        HttpUri uri("/%25some%20path?%25some%20query", "example.com");
        EXPECT(uri.path() == "/%some path");
        EXPECT(uri.query() == "%some query");
    }
    {
        HttpUri uri("http://example.com/%25some%20path?%25some%20query", "example.com");
        EXPECT(uri.path() == "/%some path");
        EXPECT(uri.query() == "%some query");
    }

    END
}

void HttpUriTest::host_case_insensitive_test()
{
    BEGIN

    {
        HttpUri uri("/", "EXAMPLE.COM");
        EXPECT(uri.host() == "example.com");
    }
    {
        HttpUri uri("http://EXAMPLE.COM", "EXAMPLE.COM");
        EXPECT(uri.host() == "example.com");
    }

    END
}

void HttpUriTest::scheme_case_insensitive_test()
{
    BEGIN
    {
        HttpUri uri("HTTP://example.com", "example.com");
    }
    END
}

#endif
