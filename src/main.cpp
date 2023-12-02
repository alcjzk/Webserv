#include <iostream>
#include <cstdlib>
#include "Config.hpp"
#include "Server.hpp"

using namespace std;

int main()
{
    try
    {
        Config  config;
        Server  server(config);

        Runtime::instance().run();
    }
    catch (const char *e)
    {
        cout << e << endl;
    }
    return (EXIT_SUCCESS);
}
