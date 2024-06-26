#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstddef>
#include <optional>

class Path
{
    public:
        typedef std::vector<std::string>::iterator       iterator;
        typedef std::vector<std::string>::const_iterator const_iterator;

        using Stat = struct stat;

        class Status : public Stat
        {
            public:
                size_t size() const;
                bool   is_regular() const;
                bool   is_directory() const;
        };

        Path() = default;
        Path(const Path& other) = default;
        Path& operator=(const Path& other) = default;
        Path(Path&& other) = default;
        Path& operator=(Path&& other) = default;
        Path(const std::string& path);
        Path(const char* path);
        Path(const_iterator first, const_iterator last);

        iterator              begin() noexcept;
        iterator              end() noexcept;
        const_iterator        cbegin() const noexcept;
        const_iterator        cend() const noexcept;
        bool                  is_root() const noexcept;
        void                  is_root(bool value) noexcept;
        std::optional<Status> status() const noexcept;
        std::optional<int>    open(int flags) const;
        std::optional<int>    open(int flags, mode_t mode) const;

        operator std::string() const;

        Path operator+(const Path& rhs) const;
        bool operator==(const Path& rhs) const;

        static Path relative(const Path& path, const Path& base);
        static Path canonical(const Path& path);

    private:
        std::vector<std::string> _segments;
        bool                     _is_root = false;
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
