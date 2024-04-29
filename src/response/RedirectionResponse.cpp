#include <string>
#include "Status.hpp"
#include "Response.hpp"
#include "RedirectionResponse.hpp"

RedirectionResponse::RedirectionResponse(const std::string& value, Status status) : Response(status)
{
    header(Header(FieldName::LOCATION, value));
}
