#include <iostream>
#include <cstdlib>
#include <exception>
#include "Log.hpp"
#include "Config.hpp"
#include "Server.hpp"

int main()
{
    try
    {
        Config config;
        Server server(config);

        Runtime::instance().run();
    }
    catch (const char* e)
    {
        ERR(e);
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        ERR(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
