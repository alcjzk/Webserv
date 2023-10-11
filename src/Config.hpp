
#ifndef CONFIG_H
# define CONFIG_H

# include <vector>
# include <string>

using namespace std;

class Config {
    public:
        Config();

        const vector<string>    &ports() const;
        void                    ports(vector<string> ports);
        int                     backlog() const;
        void                    backlog(int backlog);

    private:
        vector<string>    _ports;
        int               _backlog;
};

#endif
