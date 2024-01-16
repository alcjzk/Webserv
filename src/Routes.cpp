#include <algorithm>
#include "Routes.hpp"

using std::string;
using std::vector;

Routes::Routes(const vector<Route>& routes) : _routes(routes)
{
    std::sort(_routes.begin(), _routes.end());
}

Routes::Routes(vector<Route>&& routes) : _routes(std::move(routes))
{
    std::sort(_routes.begin(), _routes.end());
}

const Route* Routes::find(const string& uri_path) const
{
    auto route = std::find_if(_routes.cbegin(), _routes.cend(),
                              [uri_path](const auto& route) { return route.match(uri_path); });

    return route != _routes.cend() ? &(*route) : nullptr;
}

void Routes::push(Route&& route)
{
    auto before = std::find_if(_routes.cbegin(), _routes.cend(),
                               [route](const auto& other) { return other >= route; });
    _routes.insert(before, std::move(route));
}

void Routes::push(const Route& route)
{
    auto before = std::find_if(_routes.cbegin(), _routes.cend(),
                               [route](const auto& other) { return other >= route; });
    _routes.insert(before, route);
}
