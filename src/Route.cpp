#include <algorithm>
#include <exception>
#include <stdexcept>
#include <sstream>
#include "Log.hpp"
#include "Route.hpp"

const std::map<std::string, Route::Method> _method_map = {
    {"GET-ACCEPT", Route::GET}, {"POST-ACCEPT", Route::POST}, {"DELETE-ACCEPT", Route::DELETE}};

Route::Route(Path uri_path, Path fs_path) : _uri_path(uri_path)
{
    try
    {
        _fs_path = Path::canonical(fs_path);
        _priority = std::distance(_uri_path.begin(), _uri_path.end());
    }
    catch (const std::exception& error)
    {
        std::stringstream message;

        message << "Invalid route " << uri_path << " => " << fs_path << ": " << error.what();
        throw std::runtime_error(message.str());
    }
}

bool Route::match(Path uri_path) const
{
    auto pair =
        std::mismatch(uri_path.cbegin(), uri_path.cend(), _uri_path.cbegin(), _uri_path.cend());

    return pair.second == _uri_path.cend();
}

Path Route::map(Path uri_path) const
{
    return _fs_path + Path::relative(uri_path, _uri_path);
}

const Path& Route::fs_path() const
{
    return _fs_path;
}

bool Route::operator<(const Route& rhs) const
{
    return rhs._priority < _priority;
}

bool Route::operator>(const Route& rhs) const
{
    return rhs._priority > _priority;
}

bool Route::operator==(const Route& rhs) const
{
    return rhs._priority == _priority;
}

bool Route::operator<=(const Route& rhs) const
{
    return rhs._priority <= _priority;
}

bool Route::operator>=(const Route& rhs) const
{
    return rhs._priority >= _priority;
}
