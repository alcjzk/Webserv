#include <iostream>
#include "Config.hpp"
#include "Server.hpp"

using namespace std;

int main()
{
    try
    {
        Config  config;
        Server  server(config);

        server.start();
    }
    catch (const char *e)
    {
        cout << e << endl;
    }
    return (EXIT_SUCCESS);
}
