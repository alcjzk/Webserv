#include <string>
#include "Status.hpp"
#include "Response.hpp"
#include "Header.hpp"
#include "RedirectionResponse.hpp"

RedirectionResponse::RedirectionResponse(const std::string value, Status status) : Response(status)
{
    header(Header("location", value));
}
