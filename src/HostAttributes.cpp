#include "HostAttributes.hpp"

HostAttributes::HostAttributes(const std::string& hostname, const TiniNode* node)
{
    std::map<std::string, TiniNode*>& n = node->getMapValue();
    TiniNode* routes = n["routes"];
    if (!routes)
        throw std::runtime_error("Routes not in node");
    std::map<std::string, TiniNode*>& routesmap = routes->getMapValue();

    for (const auto& [key, value] : routesmap)
    {
        if (value->getStringValue().find(':') != std::string::npos)
            _routes.push(Route(key, split(value->getStringValue(), ":")[0]));
        else
            _routes.push(Route(key, value->getStringValue()));
    }
    _hostname = hostname;
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
