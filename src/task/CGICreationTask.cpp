#include <string>
#include <string.h>
#include <cassert>
#include <utility>
#include "http.hpp"
#include "FieldParams.hpp"
#include "HTTPError.hpp"
#include "Request.hpp"
#include "Reader.hpp"
#include "Connection.hpp"
#include "CGICreationTask.hpp"

using namespace cgi_creation_task;
using std::optional;
using std::string;

static void SignalhandlerChild(int sig);

// setup the environment for CGI
void CGICreationTask::SetEnv(const std::string& key, const std::string& value)
{
    std::string envVar = key + "=" + value;
    char*       envPtr = strdup(envVar.c_str()); // memory leak?
    _environment.push_back(envPtr);
}

// Append QUERY_STRING=query.. into _environment
void CGICreationTask::QueryString(const std::string& query_string)
{
    char* envPtr = strdup(query_string.c_str());
    _environment.push_back(envPtr);
}

void CGICreationTask::SignalhandlerChild(int sig)
{
    std::cerr << "Received signal (children process): " << sig << std::endl;
    std::exit(EXIT_FAILURE);
}

// convert std::vector<char*> to char**
char** CGICreationTask::Environment()
{
    char** arr = new char*[_environment.size() + 1];
    size_t i = 0;

    for (std::vector<char*>::const_iterator it = _environment.begin(); it != _environment.end();
         ++it)
    {
        arr[i++] = *it;
    }
    arr[i] = nullptr;

    return arr;
}


CGICreationTask::CGICreationTask(
    Connection&& connection, Request& request, const Path& uri, Config& config
)
{
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
    {
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }

    int pid = fork(); // Fork to create a child process
    if (pid == -1)
    {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }
    else if (pid == 0) // Child process
    {
        // TODO: signla handler
        // signal(SIGINT, SignalhandlerChild());
        close(pipe_fd[0]);               // close reading
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        close(pipe_fd[1]);

        // setup env
        SetEnv("AUTH_TYPE", "basic");
        SetEnv("REDIRECT_STATUS", "200");

        const char* path = request.request_line().request_target().c_str();
        char*       argv[] = {(char*)"/usr/local/bin/python3", (char*)path, nullptr};

        // execute
        if (execve(argv[0], argv, Environment()) == -1) // argument?
        {
            throw HTTPError(Status::INTERNAL_SERVER_ERROR);
        }
        exit(0);
    }

    WriteState write_state {
        CGIWriteTask(std::move(request), request.body(), File(pipe_fd[0]), pid, config),
        pid
    };

    state(std::move(write_state));

    // auto content_type = request.headers().get(FieldName::CONTENT_TYPE);
    // if (!content_type)
    //     throw HTTPError(Status::BAD_REQUEST); // FIXME: Unsupported media type

    // auto [media_type, parameters] = content_type->split();
    // (void)media_type;
    // const string* boundary_param = parameters.get("boundary");
    // std::string   boundary = "--" + (*boundary_param) + "\r\n";
    // std::string   boundary_end = "\r\n--" + (*boundary_param) + "--\r\n";

    // Reader reader(Buffer(std::move(request).body()));
    // if (!reader.seek(boundary.begin(), boundary.end()))
    // {
    //     // TODO: no content?
    //     assert(false);
    // }
    // reader.advance(boundary.length());

    // // Header section
    // optional<string> filename;

    // while (auto line = reader.line())
    // {
    //     if (line->empty())
    //         break;
    //     auto [name, value] = http::parse_field(*line);
    //     if (name == FieldName::CONTENT_DISPOSITION)
    //     {
    //         auto [content_disposition, parameters] = value.split();
    //         if (const string* filename_value = parameters.get("filename"))
    //         {
    //             filename = *filename_value;
    //             // remove_dquotes(*filename);
    //         }
    //     }
    // }

    // // End of headers

    // if (!filename || filename->empty())
    // {
    //     Response* response = new Response(Status::OK);
    //     response->body(R"(<a href="/uploads">Go to uploads.</a>)");
    //     response->_keep_alive = connection._keep_alive;

    //     SendState send_state{
    //         SendResponseTask(std::move(connection), response),
    //     };
    //     state(std::move(send_state));
    //     return;
    // }
    // auto              path = uploads_path + *filename;
    // string            location = uri + *filename;
    // size_t            size = reader.position(boundary_end.begin(), boundary_end.end()).value_or(0);
    // std::vector<char> content(size);

    // bool read_ok = reader.read_exact_into(size, content.data());
    // assert(read_ok);

    // UploadState upload_state{
    //     WriteTask(path, std::move(content), connection.config()),
    //     std::move(connection),
    //     location,
    // };
    // state(std::move(upload_state));
}
