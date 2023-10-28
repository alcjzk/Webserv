#ifndef REQUEST_H
# define REQUEST_H

# include <vector>
# include <stdint.h>
# include <string>
# include "Method.hpp"

class Request {
    public:
        Request(std::vector<uint8_t>& bytes);

        bool                        is_complete() const;
        bool                        is_headers_complete() const;
        void                        extend(const std::vector<uint8_t>& bytes);
        std::vector<uint8_t>        body() const;
        std::vector<std::string>    headers() const;
        std::string                 url() const;
        Method                      method() const;

    private:
        std::vector<uint8_t>        _data;
        std::vector<uint8_t>        _body;
        std::vector<std::string>    _headers;
        std::string                 _url;
        Method                      _method;
};

#endif
