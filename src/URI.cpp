#include "URI.hpp"

using std::ostream;
using std::string;

URI::URI() {}

const std::string& URI::host() const
{
    return _host;
}

const std::string& URI::path() const
{
    return _path;
}

const std::string& URI::query() const
{
    return _query;
}

URI::URI(string request_target, string host) : _host(host)
{
    size_t end;
    size_t start;

    end = request_target.find_first_of('?');
    _path = request_target.substr(0, end);
    if (end != string::npos)
    {
        _query = request_target.substr(end + 1);
    }
}

ostream& operator<<(ostream& os, const URI& uri)
{
    os << "http://" << uri.host();
    if (!uri.query().empty())
    {
        os << uri.query();
    }
    return os;
}
