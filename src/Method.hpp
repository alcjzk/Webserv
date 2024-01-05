#pragma once

#include <string>
#include <iostream>

class Method {
    public:
        Method() = default;
        Method(const std::string& str);

        std::string to_string() const;

        bool operator==(const Method& other);
    private:
        typedef enum Type
        {
            GET,
            POST,
            DELETE
        }   Type;

        static Type type_from(const std::string& str);

        Type    _type;
};

std::ostream& operator<<(std::ostream& os, const Method& method);
