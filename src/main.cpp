#include <iostream>
#include <cstdlib>
#include <exception>
#include "Log.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "TiniTree.hpp"

int main()
{
    try
    {
        Config  config;
        TiniTree tree;
        TiniNode& root = tree.getRoot();

        INFO("Printing the whole tree");
        // Printing the whole tree
        root.printContents(0, "");

        std::cout << "\n\n\n";
        INFO("Printing a nested map");
        // Printing a nested map
        root["a"]["b"]["c"].printContents(0, "");

        std::cout << "\n\n\n";
        INFO("Printing the server listing");
        // Printing servers
        for (auto n : root["http"]["servers"].getVectorValue())
            n->printContents(0, "");

        std::cout << "\n\n\n";
        INFO("Printing an individual key value pair");
        // Printing individual key-value pair
        auto v = root["root_map_value"].getStringValue();
        std::cout << "Value for key " << "root_map_value is: " << v << std::endl;
        
        Server  server(config);
        
        Runtime::instance().run();
    }
    catch (const char *e)
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
