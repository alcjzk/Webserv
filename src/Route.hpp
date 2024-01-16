#pragma once

#include <cstddef>
#include "Path.hpp"
class Route
{
    public:
        Route(Path uri_path, Path fs_path);

        bool        match(Path uri_path) const;
        Path        map(Path uri_path) const;

        const Path& fs_path() const;

        bool        operator<(const Route& rhs) const;
        bool        operator>(const Route& rhs) const;
        bool        operator==(const Route& rhs) const;
        bool        operator<=(const Route& rhs) const;
        bool        operator>=(const Route& rhs) const;

    private:
        Path      _uri_path;
        Path      _fs_path;
        ptrdiff_t _priority;
};
