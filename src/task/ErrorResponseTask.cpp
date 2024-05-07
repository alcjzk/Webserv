#include <fcntl.h>
#include <stdexcept>
#include <algorithm>
#include <exception>
#include <system_error>
#include <utility>
#include <sys/fcntl.h>
#include <string>
#include <optional>
#include <errno.h>

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
using std::unique_ptr;
using std::vector;

ErrorResponseTask::ErrorResponseTask(Connection&& connection, Status status)
    : ErrorResponseTask(std::move(connection), std::make_unique<Response>(status))
{
}

ErrorResponseTask::ErrorResponseTask(Connection&& connection, unique_ptr<Response>&& response)
{
    response->keep_alive = connection._keep_alive;
    try
    {
        std::optional<Path> error_path = connection.config().error_page(response->status());
        if (error_path)
        {
            auto error_path_status = error_path->status();
            if (error_path_status && error_path_status->is_regular())
            {
                auto fd = error_path->open(O_RDONLY | O_NONBLOCK | O_CLOEXEC);
                if (!fd)
                    throw std::system_error(errno, std::system_category());

                ReadState read_state{
                    ReadTask(fd.value(), error_path_status->size(), connection.config()),
                    std::move(connection),
                    std::move(response),
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
        WARN(
            "failed to use configured error page for status `" << response->status()
                                                               << "`: " << error.what()
        );
    }

    TemplateEngine engine(connection.config().error_str());
    engine.set_value("status", response->status().text());
    string       buf = engine.render();
    vector<char> body;

    body.resize(buf.size());
    std::copy(buf.begin(), buf.end(), body.begin());
    response->body(std::move(body));

    SendState send_state{
        SendResponseTask(std::move(connection), std::move(response)),
    };
    state(std::move(send_state));
}
