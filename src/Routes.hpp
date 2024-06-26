#pragma once

#include <vector>
#include <string>
#include "Route.hpp"

class Routes
{
    public:
        Routes() = default;
        Routes(const Routes& other) = default;
        Routes& operator=(const Routes& other) = default;
        Routes(Routes&& other) = default;
        Routes& operator=(Routes&& other) = default;
        Routes(const std::vector<Route>& routes);
        Routes(std::vector<Route>&& routes);

        const Route* find(const std::string& uri_path) const;
        void         push(Route&& route);
        void         push(const Route& route);

    private:
        std::vector<Route> _routes;
};
