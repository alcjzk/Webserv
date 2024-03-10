#include "TimeoutResponse.hpp"

TimeoutResponse::TimeoutResponse() : Response(Status::REQUEST_TIMEOUT, Connection::Close) {}
