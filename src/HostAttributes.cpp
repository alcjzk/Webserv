#include "HostAttributes.hpp"
#include "Log.hpp"
#include <algorithm>

HostAttributes::MethodMap HostAttributes::_method_map = {
    {"GET", Route::GET}, {"POST", Route::POST}, {"DELETE", Route::DELETE}};

HostAttributes::HostAttributes(const std::string& hostname, const TiniNode* node)
    : _directory_listing(false), _hostname(hostname)
{
    std::map<std::string, TiniNode*>& n = node->getMapValue();
    TiniNode*                         routes = n["routes"];
    TiniNode*                         dirlist = n["directory_listing"];

    if (!routes)
        throw std::runtime_error("Routes not in node");
    if (!dirlist)
    {
        INFO("Directory list not found for host " << hostname << " defaulting to false");
    }
    else
    {
        if (dirlist->getType() == TiniNode::T_STRING)
        {
            if (dirlist->getStringValue() != "true" && dirlist->getStringValue() != "false")
            {
                ERR("Directory list of " << hostname << " is not a boolean value");
            }
            if (dirlist->getStringValue() == "true")
                _directory_listing = true;
        }
        else
        {
            ERR("Directory list of " << hostname << " is not string type");
        }
    }
    const std::map<std::string, TiniNode*>& routes_map = routes->getMapValue();

    for (const auto& [key, value] : routes_map)
    {
        if (!value)
            throw std::runtime_error("HostAttributes: Null key in routes map");
        _assign_route(key, value);
    }
}

void HostAttributes::_assign_route(std::string key, TiniNode* value)
{
    Route     route = Route(Path(key));

    TiniNode* type = value->getMapValue()["type"];
    TiniNode* path = value->getMapValue()["path"];
    if (!path || path->getType() != TiniNode::T_STRING)
    {
        ERR("Path not defined for " << key << " skipping route definition")
        return;
    }
    route._type = Route::NORMAL;
    if (!type || type->getType() != TiniNode::T_STRING)
    {
        INFO("Type not defined for route " << key << ", defaulting to normal");
    }
    else
    {
        if (type->getStringValue() == "normal")
        {
            route._fs_path = path->getStringValue();
        }
        else if (type->getStringValue() == "redirection")
        {
            route._type = Route::REDIRECTION;
            route._redir = path->getStringValue();
            route._fs_path = std::string("./");
        }
        else
        {
            ERR("Unknown type for route " << key << ", skipping route definition");
            return;
        }
    }

    TiniNode* methods = value->getMapValue()["methods"];
    if (!methods || methods->getType() != TiniNode::T_STRING)
    {
        INFO("Methods not defined for " << key << ", route effectively forbidden")
    }
    else
    {
        const std::vector<std::string>& map_values = split(methods->getStringValue(), ",");
        route._methods = 0;
        if (!map_values.size())
        {
            INFO("Zero methods for " << key << ", route effectively forbidden")
        }
        for (const auto& str : map_values)
        {
            if (_method_map.find(str) != _method_map.end())
                route._methods |= _method_map[str];
        }
    }

    TiniNode* upload = value->getMapValue()["upload"];
    if (upload && upload->getType() == TiniNode::T_STRING)
        route._upload_directory = upload->getStringValue();

    TiniNode* default_file = value->getMapValue()["default_file"];
    if (default_file && default_file->getType() == TiniNode::T_STRING)
        route._default_file = default_file->getStringValue();
    _routes.push(route);
}

const Routes& HostAttributes::routes() const
{
    return _routes;
}

const std::string& HostAttributes::hostname() const
{
    return _hostname;
}

bool HostAttributes::dirlist() const
{
    return _directory_listing;
}
