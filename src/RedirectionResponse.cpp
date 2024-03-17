#include "RedirectionResponse.hpp"

RedirectionResponse::RedirectionResponse(const std::string value, Connection connection,
                                         Status status)
    : Response(connection, status)
{
    header(Header("Location", value));
}
