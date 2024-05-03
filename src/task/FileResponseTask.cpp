#include <utility>
#include <cstddef>
#include "File.hpp"
#include "Connection.hpp"
#include "ReadTask.hpp"
#include "FileResponseTask.hpp"

using namespace file_response_task;

FileResponseTask::FileResponseTask(Connection&& connection, File&& file, size_t size)
{
    ReadState read_state{
        ReadTask(std::move(file), size, connection.config()),
        std::move(connection),
    };
    state(std::move(read_state));
}
