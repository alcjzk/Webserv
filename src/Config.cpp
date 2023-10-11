#include "Config.hpp"

Config::Config()
{
    _ports = vector<string>(1, string("8000"));
    _backlog = 20;
}

const vector<string> &Config::ports()
{
    return _ports;
}

void Config::ports(vector<string> ports)
{
    _ports = ports;
}

int Config::backlog()
{
    return _backlog;
}

void Config::backlog(int backlog)
{
    _backlog = backlog;
}
