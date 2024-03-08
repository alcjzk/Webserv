#pragma once

#include "Path.hpp"
#include "Status.hpp"
#include "Response.hpp"

class ErrorResponse : public Response
{
    public:
        ErrorResponse(const std::string& _template_str, Status status = Status::BAD_REQUEST);
};
