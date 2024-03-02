#pragma once

#include <vector>
#include <string>
#include "HostAttributes.hpp"
#include "Status.hpp"

class Config
{
    public:
        Config() = delete;
        Config(const Config& other) = default;
        Config& operator=(const Config& other) = default;
        Config(Config&& other) = default;
        Config& operator=(Config&& other) = default;
        Config(std::map<std::string, TiniNode*>& server, std::map<std::string, TiniNode*>& root,
               std::pair<std::string, TiniNode*> first_pair);

        const std::string&                 port() const;
        const std::string&                 host() const;
        int                                backlog() const;
        const std::vector<HostAttributes>& attrs() const;
        const HostAttributes&              first_attr() const;
        size_t                             header_buffsize() const;
        std::optional<Path>                error_page(Status status) const;

    private:
        std::string                 _port;
        std::string                 _host;
        std::vector<HostAttributes> _attrs;
        HostAttributes              _first_attr;
        int                         _backlog;
        size_t                      _body_size;
        size_t                      _header_buffer_size;
        std::map<int, Path>         _error_pages;
};
