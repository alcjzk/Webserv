#include "Config.hpp"
#include "Log.hpp"

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
                if (error_path.type() != Path::REGULAR)
                {
                    ERR("Invalid error page path: " << value << ", ignoring")
                    continue;
                }
                _error_pages[error_value] = Path(value->getStringValue());
            }
        }
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
        ERR("what: " << err.what())
        throw std::runtime_error("Server is not defined as map!");
    }

    if (!header_buffer_size || header_buffer_size->getType() != TiniNode::T_STRING)
    {
        _header_buffer_size = 4096;
        INFO("Header buffer size not specified or invalid type, defaulting to 4096");
    }
    else
        _header_buffer_size = std::stoi(header_buffer_size->getStringValue());
    if (!body_size || body_size->getType() != TiniNode::T_STRING)
    {
        INFO("Body size not specified or invalid type, defaulting to 4096");
    }
    else
        _body_size = std::stoi(body_size->getStringValue());
    if (!s_port || s_port->getType() != TiniNode::T_STRING)
    {
        INFO("Port not specified or invalid type, defaulting to 8000");
    }
    else
        _port = s_port->getStringValue();
    if (!s_host || s_host->getType() != TiniNode::T_STRING)
    {
        INFO("Host not specified or invalid type, defaulting to 127.0.0.1");
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

const std::vector<HostAttributes>& Config::attrs() const
{
    return _attrs;
}

size_t Config::header_buffsize() const
{
    return _header_buffer_size;
}

std::optional<Path> Config::error_page(Status status) const
{
    auto it = _error_pages.find(status.code());
    if (it != _error_pages.end())
        return (it->second);
    return std::nullopt;
}

const HostAttributes& Config::first_attr() const
{
    return _first_attr;
}
