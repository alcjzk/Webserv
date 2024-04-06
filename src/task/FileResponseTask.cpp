#include <utility>
#include <cstddef>
#include "File.hpp"
#include "Server.hpp"
#include "ReadTask.hpp"
#include "FileResponseTask.hpp"

using namespace file_response_task;

FileResponseTask::FileResponseTask(
    File&& file, size_t size, File&& client, const Server& server, Connection connection
)
{
    ReadState read_state{
        ReadTask(std::move(file), size, server.config()),
        std::move(client),
        server,
        connection,
    };
    state(std::move(read_state));
}
