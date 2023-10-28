#ifndef METHOD_H
# define METHOD_H

# include <string>

class Method {
    public:
        Method(const std::string& str);

        std::string             to_string() const;
        static const Method     GET();
        static const Method     POST();
        static const Method     PUT();

    private:
        enum type
        {
            _GET,
            _POST,
            _PUT
        }   _type;
};

#endif
