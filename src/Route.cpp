#include <algorithm>
#include <stdexcept>
#include "Route.hpp"
#include "Log.hpp"

using AllowedMethods = Route::AllowedMethods;

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
    return (_allowed_methods.test(Method::Get));
}

bool Route::method_post() const
{
    return (_allowed_methods.test(Method::Post));
}

bool Route::method_del() const
{
    return (_allowed_methods.test(Method::Delete));
}

const AllowedMethods& Route::allowed_methods() const
{
    return _allowed_methods;
}

void Route::insert_cgi(const std::pair<std::string, TiniNode*>& extension)
{
    if (!extension.second)
        throw std::runtime_error("Null key in TiniNode");
    if (extension.second->getType() == TiniNode::T_STRING)
        _cgi_opts[extension.first] = extension.second->getStringValue();
}

std::optional<std::string> Route::get_cgi_option(std::string to_find) const
{
    INFO("Searching for cgi option")
    for (const auto& cgi_pair : _cgi_opts)
    {
        if (static_cast<std::string>(to_find).substr(static_cast<std::string>(to_find).size() - cgi_pair.first.size()) == cgi_pair.first)
        {
            INFO("Found cgi option, which is " << cgi_pair.second);
            return cgi_pair.second;
        }
    }
    return std::nullopt;
}
