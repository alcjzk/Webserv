#ifndef RESPONSE_H
# define RESPONSE_H

# include <vector>
# include <stdint.h>
# include "StatusCode.hpp"
# include "Request.hpp"

class Response {
    public:
        Response(const Request& request);

        std::vector<uint8_t>    to_bytes() const;
        StatusCode              status_code() const;

    private:
        StatusCode                  _status_code;
        std::vector<uint8_t>        _body;
        std::vector<std::string>    _headers;
};

#endif
