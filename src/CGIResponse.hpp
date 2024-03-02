#pragma once

#include "Path.hpp"
#include "Status.hpp"
#include "Request.hpp"

class CGIResponse : public Response
{
    public:
        CGIResponse(std::vector<char>&& body);
};
