#pragma once

#include <string>
#include <iostream>

class Method
{
    public:
        enum Type
        {
            Get,
            Post,
            Delete
        };

        Method() = default;
        Method(Type type) noexcept;

        operator size_t() const;
        bool operator==(const Method& other);

        std::string to_string() const;

        static const size_t MAX_LENGTH = 6;
        static const size_t COUNT = 3;

        static Method from_string(const std::string& str);

    private:
        Type _type;
};

std::ostream& operator<<(std::ostream& os, const Method& method);
