#include <sstream>
#include <fstream>
#include "http.hpp"
#include "Status.hpp"
#include "Header.hpp"
#include "HTTPVersion.hpp"
#include "Log.hpp"
#include "FileResponse.hpp"

namespace fs = std::filesystem;

FileResponse::FileResponse(const fs::path& path, Status status) : Response(status)
{
    std::filebuf*       file_rdbuf;
    size_t              file_size;
    std::vector<char>   body;

    std::ifstream file_stream(path, std::ifstream::binary);
    file_rdbuf = file_stream.rdbuf();
    file_size = file_rdbuf->pubseekoff(0, file_stream.end, file_stream.in);
    file_rdbuf->pubseekpos(0, file_stream.in);

    body.resize(file_size);
    file_rdbuf->sgetn(body.data(), file_size);

    this->body(std::move(body));
    file_stream.close();
}
