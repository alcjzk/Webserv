#include "Config.hpp"
#include "HostAttributes.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include <stdexcept>

using std::string;

Config::Config(std::map<std::string, TiniNode*>& server, std::map<std::string, TiniNode*>& root)
    : _port(string("8000")), _backlog(128), _body_size(4096) // OSX capped value for listen(2)
{
    TiniNode* body_size = root["body_size"];
    TiniNode* header_buffer_size = root["header_buffer_size"];
    TiniNode* port = server["port"];

    try
    {
        for (const auto& [key, value] : server)
        {
            if (value && value->getType() == TiniNode::T_MAP)
                _attrs.push_back(HostAttributes(key, value));
        }
    }
    catch (...)
    {
        throw std::runtime_error("Server is not defined as map!");
    }

    if (!header_buffer_size || header_buffer_size->getType() != TiniNode::T_STRING)
    {
        _header_buffer_size = 4096;
        INFO("Header buffer size not specified or invalid type, defaulting to 4096");
    }
    else
        _header_buffer_size = stoi(header_buffer_size->getStringValue());
    if (!body_size || body_size->getType() != TiniNode::T_STRING)
    {
        INFO("Body size not specified or invalid type, defaulting to 4096");
    }
    else
        _body_size = stoi(body_size->getStringValue());
    if (!port || port->getType() != TiniNode::T_STRING)
    {
        INFO("Port not specified or invalid type, defaulting to 8000");
    }
    else
        _port = port->getStringValue();
}

const string& Config::port() const
{
    return _port;
}

int Config::backlog() const
{
    return _backlog;
}

const std::vector<HostAttributes>& Config::attrs() const
{
    return _attrs;
}

size_t                             Config::header_buffsize() const
{
    return _header_buffer_size;
}
