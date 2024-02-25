#include "Config.hpp"

using std::string;
using std::vector;

Config::Config()
    : _ports(vector<string>(1, string("8000"))), _backlog(128) // OSX capped value for listen(2)
{
}

const vector<string>& Config::ports() const
{
    return _ports;
}

int Config::backlog() const
{
    return _backlog;
}

Config::Seconds Config::keepalive_timeout() const
{
    return _keepalive_timeout;
}

Config::Seconds Config::client_header_timeout() const
{
    return _client_header_timeout;
}

Config::Seconds Config::client_body_timeout() const
{
    return _client_body_timeout;
}

Config::Seconds Config::send_timeout() const
{
    return _send_timeout;
}

Config::Seconds Config::lingering_timeout() const
{
    return _lingering_timeout;
}

Config::Seconds Config::cgi_read_timeout() const
{
    return _cgi_read_timeout;
}

Config::Seconds Config::cgi_write_timeout() const
{
    return _cgi_write_timeout;
}
