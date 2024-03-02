
#include "CGIResponse.hpp"
#include <unistd.h>
#include <exception>
#include "HTTPError.hpp"

using std::vector;

CGIResponse::CGIResponse(const Path& path)
{
    extern char** environ;

    //deep copy
    std::vector<char*>


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
        signal(SIGINT, SignalhandlerChild);

        //set_env();

        close(pipe_fd[0]); // close reading
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        close(pipe_fd[1]);

        char* argv[] = {(char*)"/usr/local/bin/python3", (char *)path_string.c_str(), nullptr};
        //TODO: handel enviromental variable by calling set_env();
        //execute
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
    // fake a http request
    while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
        body.insert(body.end(), buffer, buffer + bytesRead);
    }
    close(pipe_fd[0]); // Close the read end of the pipe

    int status;
    waitpid(pid, &status, 0);
    //vector<char> delimitor = { '\n', '\n' };
    //std::vector<char>::iterator twoNewLinesPos = std::search(body.begin(), body.end(), delimitor.begin(), delimitor.end());
    //if (twoNewLinesPos != body.end()) {
    //    body.erase(body.begin(), twoNewLinesPos + 2);
    //}

    this->body(std::move(body));
}

void CGIResponse::set_env(std::string key, std::string value) {
    extern char** environ;// to std

    char** env = environ;
    while (*env != nullptr)
    {
        std::string current(*env);
        size_t pos = current.find('=');
        if (pos != std::string::npos) {
            std::string env_key = current.substr(0, pos);
            if (env_key == key) {
                *env = const_cast<char*>((" " + key + "=" + value).c_str()); // if exisit, update
                return;
            }
        }
        env++;
    }
    std::string new_env_var = key + "=" + value; // if not exisit, add
    putenv(const_cast<char*>(new_env_var.c_str()));
}

void CGIResponse::query_string(const std:string& query_string)
{
    // Append QUERY_STRING=query.. into _environment
}

void CGIResponse::SignalhandlerChild(int sig) {
    std::cerr << "Received signal (children process): " << sig << std::endl;
    std::exit(EXIT_FAILURE);
}


