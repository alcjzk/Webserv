#include <algorithm>
#include <exception>
#include <stdexcept>
#include <sstream>
#include "Log.hpp"
#include "Route.hpp"
#include <iostream>

Route::Route(Path uri_path) : _uri_path(uri_path)
{
    _priority = std::distance(_uri_path.begin(), _uri_path.end());
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

const std::optional<std::string>& Route::default_file() const
{
    return _default_file;
}

const std::optional<std::string>& Route::upload_directory() const
{
    return _upload_directory;
}

Route::RouteType Route::route_type() const
{
    return _type;
}

bool Route::method_get() const
{
    return (_methods & Route::GET);
}

bool Route::method_post() const
{
    return (_methods & Route::POST);
}

bool Route::method_del() const
{
    return (_methods & Route::DELETE);
}
