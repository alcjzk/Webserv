#pragma once

#include <vector>
#include <string>

class Config {
    public:
        Config();

        const std::vector<std::string>& ports() const;
        int                             backlog() const;

    private:
        std::vector<std::string>    _ports;
        int                         _backlog;
};
