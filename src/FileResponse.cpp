#include <fstream>
#include "FileResponse.hpp"

using std::filebuf;
using std::ifstream;
using std::vector;

namespace fs = std::filesystem;

FileResponse::FileResponse(const fs::path& path, Status status) : Response(status)
{
    filebuf*     file_rdbuf;
    size_t       file_size;
    vector<char> body;
    ifstream     file_stream;

    file_stream.exceptions(ifstream::failbit | ifstream::badbit);
    file_stream.open(path, ifstream::binary);
    file_rdbuf = file_stream.rdbuf();
    file_size = file_rdbuf->pubseekoff(0, ifstream::end, ifstream::in);
    file_rdbuf->pubseekpos(0, file_stream.in);

    body.resize(file_size);
    file_rdbuf->sgetn(body.data(), file_size);

    this->body(std::move(body));
    file_stream.close();
}
