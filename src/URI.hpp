#pragma once

#include <string>
#include <ostream>

class URI
{
    public:
        URI();
        URI(std::string uri);

        const std::string& value() const;

    private:
        std::string _value;
};

std::ostream& operator<<(std::ostream& os, const URI& uri);
