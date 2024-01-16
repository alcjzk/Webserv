#pragma once

#include "Path.hpp"
#include "Status.hpp"
#include "Response.hpp"

class FileResponse : public Response
{
    public:
        FileResponse(const Path& path, Status status = Status::OK);
};
