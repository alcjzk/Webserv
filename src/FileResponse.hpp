#pragma once

#include <filesystem>
#include "Status.hpp"
#include "Response.hpp"

class FileResponse : public Response
{
    public:
        FileResponse(const std::filesystem::path& path, Status status = Status::OK);
};
