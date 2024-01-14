#include "Config.hpp"

using std::string;

Config::Config()
    : _port("8000"), _backlog(128) // OSX capped value for listen(2)
{
}

Config::Config(std::string port)
    : _port(string(port)), _backlog(128) // OSX capped value for listen(2)
{
}

Config::Config(TiniNode& server)
    : _port(string()), _backlog(128) // OSX capped value for listen(2)
{
    try
    {
        std::map<std::string, TiniNode*>& s = server.getMapValue();
        _port = s["port"]->getStringValue();
        std::cout << "port is " << _port << std::endl;
    }
    catch (...)
    {
        throw std::runtime_error("Configuration parsing error!");
    }
}

const string& Config::port() const
{
    return _port;
}

int Config::backlog() const
{
    return _backlog;
}
