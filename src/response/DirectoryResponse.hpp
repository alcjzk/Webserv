#pragma once

#include "Path.hpp"
#include "Status.hpp"
#include "Response.hpp"

class DirectoryResponse : public Response
{
    public:
        DirectoryResponse(
            const Path& target_path, const Path& request_path, Status status = Status::OK
        );
};
