#pragma once

#include <optional>
#include <ostream>

/// RAII wrapper for a file descriptor, implicitly convertible to an int for compatability with libc
/// functions.
class File
{
    public:
        /// Destructs the type and closes any associated file descriptor.
        ~File();

        /// Constructs the type without an associated file descriptor.
        File() noexcept = default;

        /// Constructs the type from a raw file descriptor.
        File(int fd) noexcept;

        /// Constructs the type by moving.
        File(File&&) noexcept;

        /// Move assings from other. If `this` is already associated with a file descriptor, it is
        /// closed before the replacement occurs.
        File& operator=(File&& other) noexcept;

        /// Implicit conversion to raw file descriptor. If there is no associated file descriptor,
        /// -1 is returned.
        ///
        /// This conversion exists for better compatability with libc functions. The throwing
        /// alternative `fd()` is a better alternative, when `File` is known to contain an
        /// associated fd.
        operator int() const;

        /// Returns a copy of the raw file descriptor. Same as `operator int()`.
        ///
        /// @throws `std::bad_optional_access` if there is no associated file descriptor.
        int fd() const;

        /// Returns ownership of the raw file descriptor. This allows dropping the wrapper without
        /// closing the fd.
        ///
        /// @throws `std::bad_optional_access` if there is no associated file descriptor.
        int take_fd();

        /// Inserts a text representation of `File` into the output stream.
        friend std::ostream& operator<<(std::ostream& os, const File& file);

        // Disallow copy sematics.
        File(const File&) = delete;
        File& operator=(const File&) = delete;

    private:
        void close();

        std::optional<int> _fd;
};
