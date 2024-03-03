#include "TimeoutResponse.hpp"

TimeoutResponse::TimeoutResponse() : Response(Status::REQUEST_TIMEOUT)
{
    header(Header("Connection", "close"));
}
