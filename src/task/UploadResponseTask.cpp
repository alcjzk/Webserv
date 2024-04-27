#include <iterator>
#include <string>
#include <cassert>
#include <utility>
#include "FieldParams.hpp"
#include "HTTPError.hpp"
#include "Request.hpp"
#include "Reader.hpp"
#include "Connection.hpp"
#include "UploadResponseTask.hpp"

using namespace upload_response_task;
using std::optional;
using std::pair;
using std::string;

// TODO: Move somewhere (utils?)
static pair<string, FieldParams> split_field_value(const string& value)
{
    auto it = std::find(value.begin(), value.end(), ';');
    if (it == value.end())
        return {value, FieldParams()};

    auto rev_it = std::make_reverse_iterator(it);
    while (rev_it != value.rend() && (*rev_it == ' ' || *rev_it == '\t'))
    {
        rev_it--;
    }
    return {string(value.begin(), rev_it.base()), FieldParams(string(it, value.end()))};
}

// TODO: Move somewhere (utils?)
inline void remove_dquotes(string& value)
{
    if (value.size() < 2)
        return;
    if (value[0] != '"' || value[value.size() - 1] != '"')
        return;
    value = value.substr(1, value.size() - 2);
}

UploadResponseTask::UploadResponseTask(
    Connection&& connection, Request& request, const Path& uploads_path, const Path& uri
)
{
    auto content_type = request.header_by_key("content-type");
    if (!content_type)
        throw HTTPError(Status::BAD_REQUEST); // FIXME: Unsupported media type

    auto [media_type, parameters] = split_field_value(*content_type);
    (void)media_type;
    const string* boundary_param = parameters.get("boundary");
    std::string   boundary = "--" + (*boundary_param) + "\r\n";
    std::string   boundary_end = "\r\n--" + (*boundary_param) + "--\r\n";

    Reader reader(Buffer(std::move(request._body)));
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
        Header header(*line);
        if (header._name == "content-disposition")
        {
            auto [content_disposition, parameters] = split_field_value(header._value);
            if (const string* filename_value = parameters.get("filename"))
            {
                filename = *filename_value;
                remove_dquotes(*filename);
            }
        }
    }
    // End of headers

    if (filename)
    {
        auto   path = uploads_path + *filename;
        string location = uri + *filename;
        size_t size = reader.position(boundary_end.begin(), boundary_end.end()).value_or(0);
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
};
