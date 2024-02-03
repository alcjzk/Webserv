#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <optional>
#include "Path.hpp"
class Route
{
    public:
        typedef enum Method
        {
            GET,
            POST,
            DELETE,
        } Method;
        Route(Path uri_path, Path fs_path, std::optional<std::string> default_file);

        bool                              match(Path uri_path) const;
        Path                              map(Path uri_path) const;
        const std::optional<std::string>& default_file() const;

        const Path&                       fs_path() const;

        bool                              operator<(const Route& rhs) const;
        bool                              operator>(const Route& rhs) const;
        bool                              operator==(const Route& rhs) const;
        bool                              operator<=(const Route& rhs) const;
        bool                              operator>=(const Route& rhs) const;

    private:
        Path                                       _uri_path;
        Path                                       _fs_path;
        ptrdiff_t                                  _priority;
        std::map<Method, bool>                     _methods;
        static const std::map<std::string, Method> _method_map;
        std::optional<std::string>                 _default_file;
};
