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
#include "SendResponseTask.hpp"
#include "ErrorResponseTask.hpp"
#include "Status.hpp"
#include "Server.hpp"
#include "File.hpp"
#include "ReadTask.hpp"
#include "TemplateEngine.hpp"

using namespace error_response_task;

using std::string;
using std::vector;

ErrorResponseTask::ErrorResponseTask(
    File&& client, const Server& server, Status status, Connection connection
)
{
    try
    {
        std::optional<Path> error_path = server.config().error_page(status);
        if (error_path)
        {
            auto error_path_status = error_path->status();
            if (error_path_status && error_path_status->is_regular())
            {
                int fd = error_path->open(O_RDONLY);

                ReadState read_state{
                    ReadTask(fd, error_path_status->size(), server.config()),
                    std::move(client),
                    connection,
                    server,
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

    Response*      response = new Response(connection, status);
    TemplateEngine engine(server.config().error_str());
    string         buf = engine.render();
    vector<char>   body;

    body.resize(buf.size());
    std::copy(buf.begin(), buf.end(), body.begin());
    response->body(std::move(body));

    SendState send_state{
        SendResponseTask(server, std::move(client), response),
    };
    state(std::move(send_state));
}
