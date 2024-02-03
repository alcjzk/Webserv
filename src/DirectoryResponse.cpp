#include "DirectoryResponse.hpp"
#include <dirent.h>
#include <stdlib.h>
#include <sstream>
#include <cstring>
#include <algorithm>
#include "Log.hpp"

using std::filebuf;
using std::ifstream;
using std::vector;

DirectoryResponse::DirectoryResponse(const Path& path, Status status) : Response(status)
{
    std::string       cwd_path(static_cast<std::string>(path));
    const char*       cwd_cstr = cwd_path.c_str();
    DIR*              cwd_dirobj = opendir(cwd_cstr);
    struct dirent*    cwd_entry = readdir(cwd_dirobj);
    std::stringstream body;

    body << "<h1>Webserv listing for ";
    body << cwd_cstr;
    body << " </h1>\n<hr>\n<ul>\n";
    while (cwd_entry)
    {
        Path full_path = Path::canonical(path);

        INFO("Path: " << path);
        body << "  <li><a href=\"";
        if (static_cast<std::string>(full_path).back() != '/')
            body << last_uri_segment(full_path) << "/";
        else
            body << "./";
        body << cwd_entry->d_name << "\">";
        body << cwd_entry->d_name << "</a></li>\n";
        cwd_entry = readdir(cwd_dirobj);
    }
    body << "</ul>\n<hr>\n";
    closedir(cwd_dirobj);
    const std::string  body_str = body.str();
    const vector<char> body_vec(body_str.begin(), body_str.end());
    this->body(std::move(body_vec));
}

std::string DirectoryResponse::last_uri_segment(Path& full_path) const
{
    auto end = full_path.end() - 1;
    for (; end > full_path.begin(); --end)
    {
        if ((*end).length())
            break;
    }
    return (*end);
}
