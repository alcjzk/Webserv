#include "RedirectionResponse.hpp"

RedirectionResponse::RedirectionResponse(const std::string value, Status status) : Response(status)
{
    header(Header("Location", value));
}
