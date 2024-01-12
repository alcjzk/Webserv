#pragma once

#include <cstddef>
#include <filesystem>

class Route
{
    public:
        Route(std::filesystem::path uri_path, std::filesystem::path fs_path);

        bool                         match(std::filesystem::path uri_path) const;
        std::filesystem::path        map(std::filesystem::path uri_path) const;

        const std::filesystem::path& fs_path() const;

        bool                         operator<(const Route& rhs) const;
        bool                         operator>(const Route& rhs) const;
        bool                         operator==(const Route& rhs) const;
        bool                         operator<=(const Route& rhs) const;
        bool                         operator>=(const Route& rhs) const;

    private:
        std::filesystem::path _uri_path;
        std::filesystem::path _fs_path;
        ptrdiff_t             _priority;
};
