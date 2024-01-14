#pragma once

#include <vector>
#include <string>
#include "TiniTree.hpp"

class Config
{
    public:
        Config();
        Config(std::string port);
        Config(TiniNode& node);

        const std::string& port() const;
        int                             backlog() const;

    private:
        std::string _port;
        int                      _backlog;
};
