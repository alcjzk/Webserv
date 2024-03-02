#pragma once
#include <string>
#include <vector>
#include "Route.hpp"
#include "Routes.hpp"
#include "TiniNode.hpp"
#include "Path.hpp"

class HostAttributes
{
    private:
        void        _assign_route(const std::string& key, const TiniNode* value);

        bool        _directory_listing;
        std::string _hostname;
        Routes      _routes;
        Path        _directory_file;
        typedef std::map<std::string, Route::Method> MethodMap;
        static MethodMap                             _method_map;

    public:
        HostAttributes() = delete;
        HostAttributes(const HostAttributes& other) = default;
        HostAttributes& operator=(const HostAttributes& other) = default;
        HostAttributes(HostAttributes&& other) = default;
        HostAttributes& operator=(HostAttributes&& other) = default;
        HostAttributes(const std::string& hostname, const TiniNode* node);
        const Routes&      routes() const;
        const std::string& hostname() const;
        bool               dirlist() const;
};
