#pragma once

#include "Status.hpp"
#include "Path.hpp"
#include "Response.hpp"
#include <optional>

class ErrorResponse : public Response
{
    private:
        void serve_template(const std::string& _template_str, Status status);
        void serve_page(Path& error_page, Status status);

    public:
        ErrorResponse(const std::string& _template_str, Status status = Status::BAD_REQUEST);
        ErrorResponse(const Path& error_page, Status status = Status::BAD_REQUEST);
};
