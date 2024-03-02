#include "DirectoryResponse.hpp"
#include <dirent.h>
#include <stdlib.h>
#include <sstream>
#include <cstring>
#include <algorithm>
#include "Log.hpp"
#include "HTTPError.hpp"

using std::filebuf;
using std::ifstream;
using std::vector;

DirectoryResponse::DirectoryResponse(const Path& target_path, const Path& request_path,
                                     Status status)
    : Response(status)
{
    std::string cwd_path(target_path);
    const char* cwd_cstr = cwd_path.c_str();
    DIR*        cwd_dirobj = opendir(cwd_cstr);
    if (!cwd_dirobj)
    {
        ERR("Directory object for " << cwd_cstr << " not found");
        throw HTTPError(Status::NOT_FOUND);
    }
    struct dirent*    cwd_entry = readdir(cwd_dirobj);
    std::stringstream body;

    body << "<h1>Webserv listing for ";
    body << request_path;
    body << " </h1>\n<hr>\n<ul>\n";
    while (cwd_entry)
    {
        body << "  <li><a href=\"";
        if (!static_cast<std::string>(request_path).empty() &&
            static_cast<std::string>(request_path).back() != '/')
            body << last_uri_segment(request_path) << "/";
        body << cwd_entry->d_name << "\">";
        body << cwd_entry->d_name << "</a></li>\n";
        cwd_entry = readdir(cwd_dirobj);
    }
    body << "</ul>\n<hr>\n";
    closedir(cwd_dirobj);
    std::string  body_str = body.str();
    vector<char> body_vec(body_str.begin(), body_str.end());
    this->body(std::move(body_vec));
}

std::string DirectoryResponse::last_uri_segment(const Path& relative_path) const
{
    auto end = relative_path.cend() - 1;
    for (; end > relative_path.cbegin(); --end)
    {
        if ((*end).length() && *end != "/")
            break;
    }
    return (*end);
}
