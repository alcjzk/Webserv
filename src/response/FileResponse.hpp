#pragma once

#include "Path.hpp"
#include "Status.hpp"
#include "Response.hpp"

class FileResponse : public Response
{
    public:
        FileResponse(const Path& path, Connection connection, Status status = Status::OK);
};
