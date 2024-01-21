#pragma once
#include <string>
#include <vector>
#include "Route.hpp"
#include "Routes.hpp"
#include "TiniNode.hpp"
#include "TiniUtils.hpp"
#include "Path.hpp"

class HostAttributes
{
    private:
        bool        _directory_listing;
        std::string _hostname;
        Routes      _routes;
        Path        _directory_file;

    public:
        HostAttributes() = delete;
        HostAttributes(const std::string& hostname, const TiniNode* node);
        const Routes&      routes() const;
        const std::string& hostname() const;
        bool               dirlist() const;
};
