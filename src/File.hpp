#pragma once

#include <optional>
#include <ostream>

/// RAII wrapper for a file descriptor, implicitly convertible to an int for compatability with libc
/// functions.
class File
{
    public:
        /// Destructs the type and closes the associated file descriptor.
        ~File();

        /// Constructs the type from a raw file descriptor.
        File(int fd);

        /// Constructs the type by moving.
        File(File&&) noexcept;

        /// Conversion to raw file descriptor.
        operator int() const;

        /// Returns a copy of the raw file descriptor. Same as `operator int()`.
        int fd() const;

        /// Returns ownership of the raw file descriptor. This allows dropping the wrapper without
        /// closing the fd.
        int take_fd();

        File() = delete;
        File(const File&) = delete;
        File&                operator=(const File&) = delete;

        friend std::ostream& operator<<(std::ostream& os, const File& file);

    private:
        std::optional<int> _fd;
};
