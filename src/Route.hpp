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
            GET = 0b000000001,
            POST = 0b000000010,
            DELETE = 0b000000100,
        } Method;
        typedef enum RouteType
        {
            NORMAL,
            REDIRECTION
        } RouteType;

        Route(Path uri_path);

        bool                              match(Path uri_path) const;
        Path                              map(Path uri_path) const;
        const std::optional<std::string>& default_file() const;
        std::optional<std::string>        type() const;
        int                               methods() const;

        const Path&                       fs_path() const;

        bool                              operator<(const Route& rhs) const;
        bool                              operator>(const Route& rhs) const;
        bool                              operator==(const Route& rhs) const;
        bool                              operator<=(const Route& rhs) const;
        bool                              operator>=(const Route& rhs) const;

        Path                              _fs_path;
        RouteType                         _type;
        ptrdiff_t                         _priority;
        int                               _methods;
        std::optional<std::string>        _default_file;
        std::optional<std::string>        _upload_directory;

    private:
        Path _uri_path;
};
