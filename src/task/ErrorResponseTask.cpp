#include <fcntl.h>
#include <stdexcept>
#include <algorithm>
#include <exception>
#include <utility>
#include <sys/fcntl.h>
#include <string>
#include <optional>

#include "Path.hpp"
#include "Log.hpp"
#include "Response.hpp"
#include "Connection.hpp"
#include "SendResponseTask.hpp"
#include "ErrorResponseTask.hpp"
#include "Status.hpp"
#include "File.hpp"
#include "ReadTask.hpp"
#include "TemplateEngine.hpp"

using namespace error_response_task;

using std::string;
using std::vector;

ErrorResponseTask::ErrorResponseTask(Connection&& connection, Status status)
{
    try
    {
        std::optional<Path> error_path = connection.config().error_page(status);
        if (error_path)
        {
            auto error_path_status = error_path->status();
            if (error_path_status && error_path_status->is_regular())
            {
                int fd = error_path->open(O_RDONLY);

                ReadState read_state{
                    ReadTask(fd, error_path_status->size(), connection.config()),
                    std::move(connection),
                    status,
                };

                state(std::move(read_state));
                return;
            }
            else
                throw std::runtime_error("not a regular file");
        }
    }
    catch (const std::exception& error)
    {
        // FIXME: put macros in blocks to allow oneliners
        WARN("failed to use configured error page for status `" << status << "`: " << error.what());
    }

    Response* response = new Response(status);
    response->_keep_alive = connection._keep_alive;
    TemplateEngine engine(connection.config().error_str());
    string         buf = engine.render();
    vector<char>   body;

    body.resize(buf.size());
    std::copy(buf.begin(), buf.end(), body.begin());
    response->body(std::move(body));

    SendState send_state{
        SendResponseTask(std::move(connection), response),
    };
    state(std::move(send_state));
}
