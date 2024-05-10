#include "Config.hpp"
#include "Log.hpp"
#include <cstddef>
#include <fstream>

using std::string;

Config::Config(
    std::map<std::string, TiniNode*>& server, std::map<std::string, TiniNode*>& root,
    std::pair<std::string, TiniNode*> first_pair
)
    : _first_attr(HostAttributes(first_pair.first, first_pair.second))
{
    const TiniNode* body_size = root["body_size"];
    const TiniNode* header_buffer_size = root["header_buffer_size"];
    const TiniNode* s_port = server["port"];
    const TiniNode* s_host = server["host"];
    const TiniNode* errpages = server["/errorpages"];

    if (errpages && errpages->getType() == TiniNode::T_MAP)
    {
        const std::map<std::string, TiniNode*>& errpages_map = errpages->getMapValue();

        for (const auto& [key, value] : errpages_map)
        {
            if (value->getType() == TiniNode::T_STRING)
            {
                int error_value = std::stoi(key);
                if (!(error_value >= 400 && error_value <= 599))
                {
                    ERR("Error value of " << error_value
                                          << " invalid for error page configuration, ignoring")
                    continue;
                }
                Path error_path(value->getStringValue());
                auto error_path_status = error_path.status();
                if (!error_path_status || !error_path_status->is_regular())
                {
                    ERR("Invalid error page path: " << value->getStringValue() << ", ignoring")
                    continue;
                }
                _error_pages[error_value] = Path(value->getStringValue());
            }
        }
    }

    std::ifstream input_file("err/template.html");
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

size_t Config::body_size() const
{
    return _body_size;
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

Config::Seconds Config::cgi_read_timeout() const
{
    return _cgi_read_timeout;
}

Config::Seconds Config::cgi_write_timeout() const
{
    return _cgi_write_timeout;
}

Config::Seconds Config::io_read_timeout() const
{
    return _io_read_timeout;
}

Config::Seconds Config::io_write_timeout() const
{
    return _io_write_timeout;
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

std::optional<Path> Config::error_page(Status status) const
{
    auto it = _error_pages.find(status.code());
    if (it != _error_pages.end())
        return (it->second);
    return std::nullopt;
}

const std::string& Config::client_ip() const
{
    return _client_ip;
}
