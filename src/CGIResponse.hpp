#pragma once

#include "Path.hpp"
#include "Status.hpp"
#include "Request.hpp"

class CGIResponse: public Response
{
    public:
        CGIResponse(const Path& path);
        void set_env(std::string key, std::string value);
        void query_string(const std::string& query_string); //key=value

    private:
        static void SignalhandlerChild(int signum);
        std::vector<char**> _environment;
};
