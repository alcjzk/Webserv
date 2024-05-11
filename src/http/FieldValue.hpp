#pragma once

#include <string>
#include <type_traits>
#include "FieldParams.hpp"

/// A _valid_ HTTP/1.1 field name (RFC 9110).
class FieldValue
{
    public:
        /// Constructs the value from a string and validates it.
        ///
        /// @throws BAD_REQUEST - argument is not a valid field value.
        FieldValue(std::string&& value);
        /// Constructs the value from a string and validates it.
        ///
        /// @throws BAD_REQUEST - argument is not a valid field value.
        FieldValue(const std::string& value);

        /// Constructs the value from a number, skipping text validation (a number is always valid).
        template <typename Number, std::enable_if_t<std::is_integral_v<Number>, bool> = true>
        FieldValue(Number value);

        /// Case-insensitive string equality.
        bool eq_case_insensitive(const std::string& other) const;

        operator const std::string&() const noexcept;

        const std::string& operator*() const noexcept;
        const std::string* operator->() const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const FieldValue& value);

        /// Splits out field paramets from the value.
        std::pair<FieldValue, FieldParams> split() const;

        void append(const FieldValue& other);

        static const FieldValue CLOSE;

    private:
        std::string _value;
};

template <typename Number, std::enable_if_t<std::is_integral_v<Number>, bool>>
FieldValue::FieldValue(Number value) : _value(std::to_string(value))
{
}
