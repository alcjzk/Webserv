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

bool Route::match(const Path& uri_path) const
{
    auto pair =
        std::mismatch(uri_path.cbegin(), uri_path.cend(), _uri_path.cbegin(), _uri_path.cend());

    return pair.second == _uri_path.cend();
}

Path Route::map(const Path& uri_path) const
{
    return _fs_path + Path::relative(uri_path, _uri_path);
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

bool Route::method_get() const
{
    return (_allowed_methods.test(Method::GET));
}

bool Route::method_post() const
{
    return (_allowed_methods.test(Method::POST));
}

bool Route::method_del() const
{
    return (_allowed_methods.test(Method::DELETE));
}

void Route::insert_cgi(const std::pair<std::string, TiniNode*>& extension)
{
    if (!extension.second)
        throw std::runtime_error("Null key in TiniNode");
    if (extension.second->getType() == TiniNode::T_STRING)
        _cgi_opts[extension.first] = extension.second->getStringValue();
}
