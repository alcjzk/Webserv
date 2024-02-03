#include "HostAttributes.hpp"
#include "Log.hpp"
#include <algorithm>

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
                INFO("Directory list of " << hostname << " is not a boolean value");
            }
            if (dirlist->getStringValue() == "true")
                _directory_listing = true;
        }
        else
        {
            INFO("Directory list of " << hostname << " is not string type");
        }
    }
    std::map<std::string, TiniNode*>& routes_map = routes->getMapValue();

    for (const auto& [key, value] : routes_map)
    {
        if (value->getStringValue().find(':') != std::string::npos)
        {
            const std::vector<std::string>& route_values = split(value->getStringValue(), ":");
            switch(route_values.size())
            {
                case 2:
                    _routes.push(Route(Path(key), route_values[0], std::nullopt));
                    break;
                case 3:
                    _routes.push(Route(Path(key), route_values[0], route_values[1]));
                    break;
                default:
                    throw(std::runtime_error("Unknown parameter count"));
                    break;
            }
        }
        else
            _routes.push(Route(Path(key), value->getStringValue(), std::nullopt));
    }
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
