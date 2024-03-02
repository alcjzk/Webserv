#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <optional>
#include "Method.hpp"
#include "Path.hpp"
#include "TiniNode.hpp"

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
        Route() = delete;
        Route(const Route& other) = default;
        Route& operator=(const Route& other) = default;
        Route(Route&& other) = default;
        Route& operator=(Route&& other) = default;

        bool                       match(const Path& uri_path) const;
        Path                       map(const Path& uri_path) const;
        bool                       method_get() const;
        bool                       method_post() const;
        bool                       method_del() const;
        void                       insert_cgi(const std::pair<std::string, TiniNode*>& extension);

        bool                       operator<(const Route& rhs) const;
        bool                       operator>(const Route& rhs) const;
        bool                       operator==(const Route& rhs) const;
        bool                       operator<=(const Route& rhs) const;
        bool                       operator>=(const Route& rhs) const;

        Path                       _fs_path;
        RouteType                  _type;
        ptrdiff_t                  _priority;
        int                        _methods;
        std::optional<std::string> _default_file = std::nullopt;
        std::optional<std::string> _upload_directory = std::nullopt;
        std::optional<std::string> _redir = std::nullopt;

    private:
        Path                               _uri_path;
        std::map<std::string, std::string> _cgi_opts;
};
