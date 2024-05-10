
#include "CGIResponse.hpp"
#include <unistd.h>
#include <exception>
#include "HTTPError.hpp"

using std::vector;

CGIResponse::CGIResponse(std::vector<char>&& body)
{
    // header(Header("Location", value));
}
