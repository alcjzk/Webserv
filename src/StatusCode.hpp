#ifndef STATUSCODE_H
# define STATUSCODE_H

# include <string>

class StatusCode {
    public:
        StatusCode(int code);
        StatusCode(const std::string& str);

        std::string  to_string() const;
    private:
        int _code;
};

#endif
