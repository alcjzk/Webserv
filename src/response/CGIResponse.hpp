#pragma once

#include "Path.hpp"
#include "Status.hpp"
#include "Request.hpp"
#include "Response.hpp"

// set the header and body
class CGIResponse : public Response
{
    public:
        CGIResponse(std::vector<char>&& body);
};
