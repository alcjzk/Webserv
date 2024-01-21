#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include "TiniTree.hpp"
#include "Route.hpp"
#include "HostAttributes.hpp"

class Config
{
    public:
        Config() = delete;
        Config(std::map<std::string, TiniNode*>& server, std::map<std::string, TiniNode*>& root);

        const std::string&                 port() const;
        int                                backlog() const;
        const std::vector<HostAttributes>& attrs() const;
        size_t                             header_buffsize() const;

    protected:
        std::string                 _port;
        std::vector<HostAttributes> _attrs;
        int                         _backlog;
        size_t                      _body_size;
        size_t                      _header_buffer_size;
};
