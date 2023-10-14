#ifndef CONNECTION_H
# define CONNECTION_H

# include "Request.hpp"
# include "Response.hpp"

class Connection {
    public:
        Request    &receive() const;
        void       send(Response &response) const;
};

#endif
