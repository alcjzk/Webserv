#pragma once

#ifdef LOG_ENABLE

# include <sstream>

class Log
{
    public:
        static void info(const std::string& message);
        static void warn(const std::string& message);
        static void error(const std::string& message, const char* file, int line);
};

# ifdef LOGLEVEL_ERR
#  define LOG_ENABLE_ERR
# endif

# ifdef LOGLEVEL_WARN
#  define LOG_ENABLE_ERR
#  define LOG_ENABLE_WARN
# endif

# ifdef LOGLEVEL_INFO
#  define LOG_ENABLE_ERR
#  define LOG_ENABLE_WARN
#  define LOG_ENABLE_INFO
# endif

#endif

#ifdef LOG_ENABLE_INFO
# define INFO(message) {std::ostringstream log_stream_; log_stream_ << message; Log::info(log_stream_.str());}
# undef LOG_ENABLE_INFO
#else
# define INFO(x)
#endif

#ifdef LOG_ENABLE_WARN
# define WARN(message) {std::ostringstream log_stream_; log_stream_ << message; Log::warn(log_stream_.str());}
# undef LOG_ENABLE_WARN
#else
# define WARN(x)
#endif

#ifdef LOG_ENABLE_ERR
# define ERR(message) {std::ostringstream log_stream_; log_stream_ << message; Log::error(log_stream_.str(), __FILE__, __LINE__);}
# undef LOG_ENABLE_ERR
#else
# define ERR(x)
#endif
