#pragma once

#include <string>

/// A _valid_ HTTP/1.1 field name (RFC 9110).
class FieldName
{
    public:
        /// String constructor.
        ///
        /// @throws
        /// BAD_REQUEST - argument is not a valid field name.
        explicit FieldName(std::string&& name);
        explicit FieldName(const std::string& name);

        operator const std::string&() const noexcept;

        const std::string& operator*() const noexcept;
        const std::string* operator->() const noexcept;

        friend bool operator==(const FieldName& lhs, const FieldName& rhs) noexcept;
        friend bool operator!=(const FieldName& lhs, const FieldName& rhs) noexcept;
        friend bool operator==(const FieldName& lhs, const std::string& rhs) noexcept;
        friend bool operator!=(const FieldName& lhs, const std::string& rhs) noexcept;

        friend std::ostream& operator<<(std::ostream& os, const FieldName& name);

        // Predefined field names.

        static const FieldName HOST;
        static const FieldName LOCATION;
        static const FieldName CONNECTION;
        static const FieldName CONTENT_TYPE;
        static const FieldName CONTENT_LENGTH;
        static const FieldName TRANSFER_ENCODING;
        static const FieldName CONTENT_DISPOSITION;

    private:
        std::string _value;
};

template <>
struct std::hash<FieldName>
{
    public:
        std::size_t operator()(const FieldName& name) const noexcept
        {
            return std::hash<std::string>{}(*name);
        }
};

#ifdef TEST

class FieldNameTest : public FieldName
{
    public:
        static void name_is_case_insensitive_test();
};

#endif
