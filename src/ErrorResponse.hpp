#pragma once

#include "Status.hpp"
#include "Path.hpp"
#include "Response.hpp"

class ErrorResponse : public Response
{
    public:
        ErrorResponse(const std::string& template_str, Status status = Status::BAD_REQUEST);
        ErrorResponse(const Path& error_page, Status status = Status::BAD_REQUEST);
};
