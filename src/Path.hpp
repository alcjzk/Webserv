#pragma once

#include <vector>
#include <string>
#include <ostream>

class Path
{
    public:
        typedef std::vector<std::string>::iterator       iterator;
        typedef std::vector<std::string>::const_iterator const_iterator;

        enum Type
        {
            NONE,
            NOT_FOUND,
            REGULAR,
            DIRECTORY,
            CHARACTER,
            BLOCK,
            FIFO,
            LINK,
            SOCKET,
        };

        Path() = default;
        Path(const std::string& path);
        Path(const char* path);

        iterator       begin() noexcept;
        iterator       end() noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;
        Type           type() const;

        operator std::string() const;

        Path        operator+(const Path& rhs) const;

        static Path relative(const Path& path, const Path& base);
        static Path canonical(const Path& path);

    private:
        std::vector<std::string> _segments;
        Type                     _type = NONE;
};

std::ostream& operator<<(std::ostream& os, const Path& path);
