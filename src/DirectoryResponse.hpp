#pragma once

#include "Path.hpp"
#include "Status.hpp"
#include "Response.hpp"

class DirectoryResponse : public Response
{
    public:
        DirectoryResponse(const Path& target_path, const Path& request_path,
                          Status status = Status::OK);

    private:
        std::string last_uri_segment(const Path& full_path) const;
};
