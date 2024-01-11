#pragma once

#include <string>
#include <ostream>

class URI
{
    public:
        URI();
        // URI(std::string uri); // From request target and host
        URI(std::string request_target, std::string host);

        // const std::string& value() const;

        const std::string& host() const;
        const std::string& path() const;
        const std::string& query() const;

    private:
        std::string _host;  // Match server host
        std::string _path;  // Match resource
        std::string _query; // Passed
                            // std::string _scheme; // Probably unnecessary
};

std::ostream& operator<<(std::ostream& os, const URI& uri);
