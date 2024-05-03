#pragma once

#include <string>
#include <iostream>

class Method
{
    public:
        static const size_t MAX_LENGTH = 6;

        Method() = default;
        Method(const std::string& str);
        static const size_t COUNT = 3;
        operator size_t() const;

        std::string to_string() const;

        bool operator==(const Method& other);
        typedef enum Type
        {
            GET,
            POST,
            DELETE
        } Type;
        static Type type_from(const std::string& str);

    private:
        Type _type;
};

std::ostream& operator<<(std::ostream& os, const Method& method);
