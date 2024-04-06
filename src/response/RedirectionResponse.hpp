#pragma once

#include "Status.hpp"
#include "Response.hpp"

class RedirectionResponse : public Response
{
    public:
        RedirectionResponse(
            const std::string value, Connection connection,
            Status status = Status::MOVED_PERMANENTLY
        );
};
