#include <string>
#include <cassert>
#include <utility>
#include "http.hpp"
#include "FieldParams.hpp"
#include "HTTPError.hpp"
#include "Request.hpp"
#include "Reader.hpp"
#include "Connection.hpp"
#include "CGIResponseTask.hpp"
using namespace cgi_response_task;
using std::optional;
using std::string;

CGIResponseTask::CGIResponseTask(
    Connection&& connection, Request& request, const Path& uri
)
{
    auto content_type = request.headers().get(FieldName::CONTENT_TYPE);
    if (!content_type)
        throw HTTPError(Status::BAD_REQUEST); // FIXME: Unsupported media type

    auto [media_type, parameters] = content_type->split();
    (void)media_type;
    const string* boundary_param = parameters.get("boundary");
    std::string   boundary = "--" + (*boundary_param) + "\r\n";
    std::string   boundary_end = "\r\n--" + (*boundary_param) + "--\r\n";

    Reader reader(Buffer(std::move(request).body()));
    if (!reader.seek(boundary.begin(), boundary.end()))
    {
        // TODO: no content?
        assert(false);
    }
    reader.advance(boundary.length());

    // Header section
    optional<string> filename;

    while (auto line = reader.line())
    {
        if (line->empty())
            break;
        auto [name, value] = http::parse_field(*line);
        if (name == FieldName::CONTENT_DISPOSITION)
        {
            auto [content_disposition, parameters] = value.split();
            if (const string* filename_value = parameters.get("filename"))
            {
                filename = *filename_value;
                remove_dquotes(*filename);
            }
        }
    }

    // End of headers

    if (!filename || filename->empty())
    {
        Response* response = new Response(Status::OK);
        response->body(R"(<a href="/uploads">Go to uploads.</a>)");
        response->_keep_alive = connection._keep_alive;

        SendState send_state{
            SendResponseTask(std::move(connection), response),
        };
        state(std::move(send_state));
        return;
    }
    auto              path = uploads_path + *filename;
    string            location = uri + *filename;
    size_t            size = reader.position(boundary_end.begin(), boundary_end.end()).value_or(0);
    std::vector<char> content(size);

    bool read_ok = reader.read_exact_into(size, content.data());
    assert(read_ok);

    UploadState upload_state{
        WriteTask(path, std::move(content), connection.config()),
        std::move(connection),
        location,
    };
    state(std::move(upload_state));
}
