#include "ErrorResponse.hpp"
#include "Log.hpp"
#include "HTTPError.hpp"
#include "TemplateEngine.hpp"

ErrorResponse::ErrorResponse(const std::string& _template_str, Status status) : Response(status)
{
    TemplateEngine    engine(_template_str);
    size_t            file_size;
    std::vector<char> body;

    engine.set_value("status", status.text());
    std::string buf = engine.render();
    body.resize(buf.size());
    std::copy(buf.begin(), buf.end(), body.begin());
    this->body(std::move(body));
}
