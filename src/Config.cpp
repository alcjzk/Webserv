#include "Config.hpp"
#include "Log.hpp"
#include <fstream>

using std::string;

Config::Config(std::map<std::string, TiniNode*>& server, std::map<std::string, TiniNode*>& root,
               std::pair<std::string, TiniNode*> first_pair)
    : _port(string("8000")), _host(string("127.0.0.1")),
      _first_attr(HostAttributes(first_pair.first, first_pair.second)), _backlog(128),
      _body_size(4096)
{
    const TiniNode* body_size = root["body_size"];
    const TiniNode* header_buffer_size = root["header_buffer_size"];
    const TiniNode* s_port = server["port"];
    const TiniNode* s_host = server["host"];

    std::ifstream   input_file("err/template.html");
    if (!input_file.is_open())
    {
        ERR("Config: Error template file not found, defaulting")
    }
    else
    {
        std::stringstream buffer;

        buffer << input_file.rdbuf();
        _error_template = buffer.str();
    }
    try
    {
        for (const auto& [key, value] : server)
        {
            if (value && value->getType() == TiniNode::T_MAP && key[0] != '/')
                _attrs.push_back(HostAttributes(key, value));
        }
    }
    catch (const std::exception& err)
    {
        ERR("Config: what: " << err.what())
        throw std::runtime_error("Server is not defined as map!");
    }

    if (!header_buffer_size || header_buffer_size->getType() != TiniNode::T_STRING)
    {
        _header_buffer_size = 4096;
        INFO("Config: Header buffer size not specified or invalid type, defaulting to 4096");
    }
    else
        _header_buffer_size = std::stoi(header_buffer_size->getStringValue());
    if (!body_size || body_size->getType() != TiniNode::T_STRING)
    {
        INFO("Config: Body size not specified or invalid type, defaulting to 4096");
    }
    else
        _body_size = std::stoi(body_size->getStringValue());
    if (!s_port || s_port->getType() != TiniNode::T_STRING)
    {
        INFO("Config: Port not specified or invalid type, defaulting to 8000");
    }
    else
        _port = s_port->getStringValue();
    if (!s_host || s_host->getType() != TiniNode::T_STRING)
    {
        INFO("Config: Host not specified or invalid type, defaulting to 127.0.0.1");
    }
    else
        _host = s_host->getStringValue();
}

const string& Config::port() const
{
    return _port;
}

const string& Config::host() const
{
    return _host;
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

const std::vector<HostAttributes>& Config::attrs() const
{
    return _attrs;
}

size_t Config::header_buffsize() const
{
    return _header_buffer_size;
}

const HostAttributes& Config::first_attr() const
{
    return _first_attr;
}

const std::string& Config::error_str() const
{
    return _error_template;
}
