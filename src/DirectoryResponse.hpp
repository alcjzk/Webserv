#pragma once

#include "Path.hpp"
#include "Status.hpp"
#include "Response.hpp"

class DirectoryResponse : public Response
{
    public:
        DirectoryResponse(const Path& path, Status status = Status::OK);
    private:
        std::string last_uri_elem() const;
        std::string last_uri_segment(Path& full_path) const;
};
