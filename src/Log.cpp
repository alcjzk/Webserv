#ifdef LOG_ENABLE

#include <iostream>
#include "Log.hpp"

#define RED              "\e[0;31m"
#define YELLOW           "\e[0;33m"
#define CYAN             "\e[0;36m"
#define NORMAL           "\e[0m"
#define PURPLE_UNDERLINE "\e[4;35m"

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
