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
        void        _assign_route(std::string key, TiniNode* value);

        bool        _directory_listing;
        std::string _hostname;
        Routes      _routes;
        Path        _directory_file;
        typedef std::map<std::string, Route::Method> MethodMap;
        static MethodMap                             _method_map;

    public:
        HostAttributes() = delete;
        HostAttributes(const std::string& hostname, const TiniNode* node);
        const Routes&      routes() const;
        const std::string& hostname() const;
        bool               dirlist() const;
};
