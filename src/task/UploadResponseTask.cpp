#include <string>
#include <cassert>
#include <utility>
#include "http.hpp"
#include "FieldParams.hpp"
#include "HTTPError.hpp"
#include "Request.hpp"
#include "Reader.hpp"
#include "Connection.hpp"
#include "UploadResponseTask.hpp"

using namespace upload_response_task;
using std::optional;
using std::string;

UploadResponseTask::UploadResponseTask(
    Connection&& connection, Request& request, const Path& uploads_path, const Path& uri
)
{
    auto content_type = request.headers().get(FieldName::CONTENT_TYPE);
    if (!content_type)
    {
        // TODO: This should probably set Accept: header
        throw HTTPError(Status::UNSUPPORTED_MEDIA_TYPE);
    }

    auto [media_type, parameters] = content_type->split();
    if (*media_type != "multipart/form-data")
    {
        // TODO: This should probably set Accept: header
        throw HTTPError(Status::UNSUPPORTED_MEDIA_TYPE);
    }

    const string* boundary_param = parameters.get("boundary");
    if (!boundary_param)
        throw HTTPError(Status::BAD_REQUEST);
    std::string boundary = "--" + (*boundary_param) + "\r\n";
    std::string boundary_end = "\r\n--" + (*boundary_param) + "--\r\n";

    Reader reader(Buffer(std::move(request).body()));
    if (!reader.seek(boundary.begin(), boundary.end()))
    {
        auto response = std::make_unique<Response>(Status::OK);
        response->body(R"(<a href="/uploads">Go to uploads.</a>)");
        response->_keep_alive = connection._keep_alive;

        SendState send_state{
            SendResponseTask(std::move(connection), std::move(response)),
        };
        state(std::move(send_state));
    }

    // Header section
    reader.advance(boundary.length());
    optional<string> filename;

    FieldMap headers;
    while (auto line = reader.line())
    {
        if (line->empty())
            break;
        if (!headers.insert(http::parse_field(*line)))
            throw HTTPError(Status::BAD_REQUEST);
        auto [name, value] = http::parse_field(*line);
        if (name == FieldName::CONTENT_DISPOSITION)
        {
            auto [content_disposition, parameters] = value.split();
        }
    }

    const auto content_disposition = headers.get(FieldName::CONTENT_DISPOSITION);
    if (!content_disposition)
        throw HTTPError(Status::BAD_REQUEST);
    auto [disposition_type, disposition_params] = content_disposition->split();
    if (*disposition_type != "form-data")
        throw HTTPError(Status::BAD_REQUEST);
    if (!disposition_params.get("name"))
        throw HTTPError(Status::BAD_REQUEST);

    if (const string* filename_value = disposition_params.get("filename"))
    {
        filename = *filename_value;
    }

    if (!filename || filename->empty())
    {
        auto response = std::make_unique<Response>(Status::OK);
        response->body(R"(<a href="/uploads">Go to uploads.</a>)");
        response->_keep_alive = connection._keep_alive;

        SendState send_state{
            SendResponseTask(std::move(connection), std::move(response)),
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
