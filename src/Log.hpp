#pragma once

// TODO: Impl into source file

// #define LOG_ENABLE
// #define LOGLEVEL_INFO

#ifdef LOG_ENABLE
# undef LOG_ENABLE

# include <iostream>
# include <sstream>

# define RED "\e[0;31m"
# define YELLOW "\e[0;33m"
# define CYAN "\e[0;36m"
# define NORMAL "\e[0m"
# define PURPLE_UNDERLINE "\e[4;35m"

class Logger
{
    public:
        static void info(const std::string& message)
        {
            std::cout << "[" << CYAN << "info" << NORMAL << "] " << message << '\n';
        }
        static void warn(const std::string& message)
        {
            std::cerr << "[" << YELLOW << "warn" << NORMAL << "] " << message << '\n';
        }
        static void error(const std::string& message, const char* file, int line)
        {
            std::cerr << "[" << RED << "error" << NORMAL << "] " << message << " (" << PURPLE_UNDERLINE << "in " << file << ':' << line << NORMAL << ')' << '\n';
        }
};

# undef RED
# undef YELLOW
# undef CYAN
# undef NORMAL

# ifdef LOGLEVEL_ERR
#  undef LOGLEVEL_ERR
#  define LOG_ENABLE_ERR
# endif

# ifdef LOGLEVEL_WARN
#  undef LOGLEVEL_WARN
#  define LOG_ENABLE_ERR
#  define LOG_ENABLE_WARN
# endif

# ifdef LOGLEVEL_INFO
#  undef LOGLEVEL_INFO
#  define LOG_ENABLE_ERR
#  define LOG_ENABLE_WARN
#  define LOG_ENABLE_INFO
# endif

#endif

#ifdef LOG_ENABLE_INFO
# define INFO(message) Logger::info(static_cast<std::ostringstream>((std::ostringstream() << message)).str())
# undef LOG_ENABLE_INFO
#else
# define INFO(x)
#endif

#ifdef LOG_ENABLE_WARN
# define WARN(message) Logger::warn(static_cast<std::ostringstream>((std::ostringstream() << message)).str())
# undef LOG_ENABLE_WARN
#else
# define WARN(x)
#endif

#ifdef LOG_ENABLE_ERR
# define ERR(message) Logger::error(static_cast<std::ostringstream>((std::ostringstream() << message)).str(), __FILE__, __LINE__)
# undef LOG_ENABLE_ERR
#else
# define ERR(x)
#endif
