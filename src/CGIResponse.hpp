#pragma once

#include "Path.hpp"
#include "Status.hpp"
#include "Request.hpp"

class CGIResponse: public Response
{
    public:
        CGIResponse(const Path& path);

        //void set_env(std::string key, std::string value);
};
