#pragma once

#include "Status.hpp"
#include "Response.hpp"

class RedirectionResponse : public Response
{
    public:
        RedirectionResponse(const std::string value, Status status = Status::PERM_REDIR);
};
