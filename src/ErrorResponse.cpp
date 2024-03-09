#include "ErrorResponse.hpp"
#include "Log.hpp"
#include "HTTPError.hpp"
#include "TemplateEngine.hpp"
#include <fstream>

using std::filebuf;
using std::ifstream;
using std::vector;

ErrorResponse::ErrorResponse(const Path& error_page, Status status) : Response(status)
{
    filebuf*     file_rdbuf;
    size_t       file_size;
    vector<char> body;
    ifstream     file_stream;

    file_stream.exceptions(ifstream::failbit | ifstream::badbit);
    file_stream.open(error_page, ifstream::binary);
    file_rdbuf = file_stream.rdbuf();
    file_size = file_rdbuf->pubseekoff(0, ifstream::end, ifstream::in);
    file_rdbuf->pubseekpos(0, file_stream.in);
    body.resize(file_size);
    file_rdbuf->sgetn(body.data(), file_size);

    TemplateEngine engine(std::string(body.begin(), body.end()));
    engine.set_value("status", status.text());
    std::string buf = engine.render();
    body.resize(buf.size());

    std::copy(buf.begin(), buf.end(), body.begin());
    this->body(std::move(body));
    file_stream.close();
}

ErrorResponse::ErrorResponse(const std::string& template_str, Status status)
{
    TemplateEngine    engine(template_str);
    std::vector<char> body;

    engine.set_value("status", status.text());
    std::string buf = engine.render();
    body.resize(buf.size());
    std::copy(buf.begin(), buf.end(), body.begin());
    this->body(std::move(body));
}
