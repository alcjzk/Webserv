#include "Status.hpp"
#include "Response.hpp"
#include "TimeoutResponse.hpp"

TimeoutResponse::TimeoutResponse() : Response(Status::REQUEST_TIMEOUT)
{
    _keep_alive = false;
}
