#include <algorithm>
#include <exception>
#include <stdexcept>
#include <sstream>
#include "Log.hpp"
#include "Route.hpp"

Route::Route(std::filesystem::path uri_path, std::filesystem::path fs_path) : _uri_path(uri_path)
{
    try
    {
        _fs_path = std::filesystem::canonical(fs_path);
        _priority = std::distance(_uri_path.begin(), _uri_path.end());
    }
    catch (const std::exception& error)
    {
        std::stringstream message;

        message << "Invalid route " << uri_path << " => " << fs_path << ": " << error.what();
        throw std::runtime_error(message.str());
    }
}

bool Route::match(std::filesystem::path uri_path) const
{
    auto pair = std::mismatch(uri_path.begin(), uri_path.end(), _uri_path.begin(), _uri_path.end());

    return pair.second == _uri_path.end();
}

std::filesystem::path Route::map(std::filesystem::path uri_path) const
{
    return _fs_path / std::filesystem::relative(uri_path, _uri_path);
}

const std::filesystem::path& Route::fs_path() const
{
    return _fs_path;
}

bool Route::operator<(const Route& rhs) const
{
    return _priority < rhs._priority;
}

bool Route::operator>(const Route& rhs) const
{
    return _priority > rhs._priority;
}

bool Route::operator==(const Route& rhs) const
{
    return _priority == rhs._priority;
}

bool Route::operator<=(const Route& rhs) const
{
    return _priority <= rhs._priority;
}

bool Route::operator>=(const Route& rhs) const
{
    return _priority >= rhs._priority;
}
