#ifndef METHOD_H
# define METHOD_H

# include <string>
# include <iostream>
# include "Reader.hpp"

class Method {
    public:
        Method();
        Method(const std::string& str);
        Method(Reader& reader);

        std::string             to_string() const;
        static const Method     deserialize(Reader& reader);

        bool operator==(const Method& other);


        static const size_t STRING_MAX_LENGTH = 7;
    private:
        typedef enum Type
        {
            Get,
            Post,
            Delete
        }   Type;
        Type   _type;

        static Type _type_from(const std::string& str);
};

std::ostream& operator<<(std::ostream& os, const Method& method);

#endif
