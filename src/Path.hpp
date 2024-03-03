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
            UNKNOWN,
        };

        Path() = default;
        Path(const Path& other) = default;
        Path& operator=(const Path& other) = default;
        Path(Path&& other) = default;
        Path& operator=(Path&& other) = default;
        Path(const std::string& path);
        Path(const char* path);
        Path(const_iterator first, const_iterator last);

        iterator       begin() noexcept;
        iterator       end() noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;
        Type           type();
        bool           is_root() const noexcept;
        void           is_root(bool value) noexcept;

        operator std::string() const;

        Path        operator+(const Path& rhs) const;
        bool        operator==(const Path& rhs) const;

        static Path relative(const Path& path, const Path& base);
        static Path canonical(const Path& path);

    private:
        Type                     fetch_type() const;

        std::vector<std::string> _segments;
        bool                     _is_root = false;
        Type                     _type = NONE;
};

std::ostream& operator<<(std::ostream& os, const Path& path);

#ifdef TEST

class PathTest : public Path
{
    public:
        static void canonical_test();
        static void repeated_delim_test();
};

#endif
