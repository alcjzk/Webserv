#ifdef LOG_ENABLE

#include <iostream>
#include "Log.hpp"

#define RED              "\033[0;31m"
#define YELLOW           "\033[0;33m"
#define CYAN             "\033[0;36m"
#define NORMAL           "\033[0m"
#define PURPLE_UNDERLINE "\033[4;35m"

using std::cerr;
using std::cout;
using std::string;

void Log::info(const string& message)
{
    cout << "[" << CYAN << "info" << NORMAL << "] " << message << '\n';
}

void Log::warn(const string& message)
{
    cerr << "[" << YELLOW << "warn" << NORMAL << "] " << message << '\n';
}

void Log::error(const string& message, const char* file, int line)
{
    cerr << "[" << RED << "error" << NORMAL << "] " << message << " (" << PURPLE_UNDERLINE << "in "
         << file << ':' << line << NORMAL << ')' << '\n';
}

#endif
