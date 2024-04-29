#pragma once

#include <string>
#include "FieldParams.hpp"

class FieldValue
{
    public:
        FieldValue(const char* name);
        FieldValue(std::string&& value);
        FieldValue(const std::string& value);

        operator const std::string&() const noexcept;

        const std::string& operator*() const noexcept;
        const std::string* operator->() const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const FieldValue& value);

        std::pair<FieldValue, FieldParams> split() const;

    private:
        std::string _value;
};
