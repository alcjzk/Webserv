#include "URI.hpp"

using std::string;
using std::ostream;

URI::URI()
{

}

URI::URI(string value) : _value(value)
{

}

const string& URI::value() const
{
    return _value;
}

ostream& operator<<(ostream& os, const URI& uri)
{
    return os << uri.value();
}
