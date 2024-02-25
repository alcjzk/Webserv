#include "Config.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "TiniTree.hpp"
#include "Runtime.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
    try
    {
        Config    config;
        TiniTree  tree;
        TiniNode& root = tree.getRoot();
        TiniNode  test = root["http"];
        TiniNode  funny = test;

        INFO("Printing the copied test");
        test.printContents(0, "");

        INFO("Printing the whole tree");
        root.printContents(0, "");

        std::cout << "\n\n\n";
        INFO("Printing a nested map");
        root["a"]["b"]["c"].printContents(0, "");

        std::cout << "\n\n\n";
        INFO("Printing the server listing");
        for (auto n : root["http"]["servers"].getVectorValue())
            n->printContents(0, "");

        std::cout << "\n\n\n";
        INFO("Printing an individual key value pair");
        auto v = root["root_map_value"].getStringValue();
        std::cout << "Value for key "
                  << "root_map_value is: " << v << std::endl;

        Server server(config);

        Runtime::run();
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

#ifdef TEST
    system("leaks debug");
#endif

    return EXIT_SUCCESS;
}
