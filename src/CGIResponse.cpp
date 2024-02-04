
#include "CGIResponse.hpp"
#include <unistd.h>
#include <exception>
#include "HTTPError.hpp"

using std::vector;

CGIResponse::CGIResponse(const Path& path)
{
    extern char** environ;

    std::string  path_string = static_cast<std::string>(path);

    // Create a pipe to capture the standard output of the CGI process
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }

    // Fork to create a child process
    int pid = fork();
    if (pid == -1)
    {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        throw HTTPError(Status::INTERNAL_SERVER_ERROR);
    }
    else if (pid == 0) // Child process
    {
        close(pipe_fd[0]); // close reading
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        close(pipe_fd[1]);

        // path to be fixed
        std::string absolute_path = path_string;
        char* argv[] = {(char*)"/usr/local/bin/python3", (char *)path_string.c_str(), nullptr};
        //TODO: handel enviromental variable
        if (execve(argv[0], argv, environ) == -1) // argument?
        {
            throw HTTPError(Status::INTERNAL_SERVER_ERROR);
        }
        exit(0);
    }

    vector<char> body;
    close(pipe_fd[1]); // Close the write end of the pipe

    // Read from the pipe (stdout of the CGI process)
    char buffer[4096];
    size_t bytesRead;
    while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
        body.insert(body.end(), buffer, buffer + bytesRead);
    }
    close(pipe_fd[0]); // Close the read end of the pipe

    int status;
    waitpid(pid, &status, 0);
    // TODO: everything followed by two new line will be save into body
    this->body(std::move(body));
}

//CGIResponse::set_env(std::string key, std::string value)
//{

//}



