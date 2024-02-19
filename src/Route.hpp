#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <optional>
#include "Method.hpp"
#include "Path.hpp"

class Route
{
    public:
        typedef enum Method
        {
            GET = 0b001,
            POST = 0b010,
            DELETE = 0b100,
        } Method;
        typedef enum RouteType
        {
            NORMAL,
            REDIRECTION
        } RouteType;

        Route(Path uri_path);

        bool                              match(Path uri_path) const;
        Path                              map(Path uri_path) const;
        bool                              method_get() const;
        bool                              method_post() const;
        bool                              method_del() const;

        bool                              operator<(const Route& rhs) const;
        bool                              operator>(const Route& rhs) const;
        bool                              operator==(const Route& rhs) const;
        bool                              operator<=(const Route& rhs) const;
        bool                              operator>=(const Route& rhs) const;

        Path                              _fs_path;
        RouteType                         _type;
        ptrdiff_t                         _priority;
        int                               _methods;
        std::optional<std::string>        _default_file = std::nullopt;
        std::optional<std::string>        _upload_directory = std::nullopt;
        std::optional<std::string>        _redir = std::nullopt;

    private:
        Path _uri_path;
};
