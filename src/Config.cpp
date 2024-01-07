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
