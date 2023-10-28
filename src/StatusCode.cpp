#include "StatusCode.hpp"

using std::string;

string StatusCode::to_string() const
{
    switch (_code)
    {
        case 404:
            return string("404 Not Found");
        default:
            throw "Aw nyaw :(";
    }
}
