#include "TimeoutResponse.hpp"

TimeoutResponse::TimeoutResponse() : Response(Connection::Close, Status::REQUEST_TIMEOUT) {}
