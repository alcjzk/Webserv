#include "Config.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "TiniTree.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
    try
    {
        TiniTree  tree;
        TiniNode& root = tree.getRoot();
        TiniNode  servers = root["servers"];
        std::vector<Server *> v_servers;
        std::vector<Config *> v_configs;

        for (const auto& [key, val] : servers.getMapValue())
        {
            v_configs.push_back(new Config(*val));
            v_servers.push_back(new Server(*v_configs.back()));
        }
        // root.printContents(0, "");
        // std::map<std::string, TiniNode *> server_maps = servers.getMapValue();
        // for ( const auto &myPair : server_maps ) {
        //     
        // }
        // INFO("Printing the copied test");
        // test.printContents(0, "");

        // INFO("Printing the whole tree");
        // root.printContents(0, "");

        // std::cout << "\n\n\n";
        // INFO("Printing a nested map");
        // root["a"]["b"]["c"].printContents(0, "");

        // std::cout << "\n\n\n";
        // INFO("Printing the server listing");
        // for (auto n : root["http"]["servers"].getVectorValue())
        //     n->printContents(0, "");

        // std::cout << "\n\n\n";
        // INFO("Printing an individual key value pair");
        // auto v = root["root_map_value"].getStringValue();
        // std::cout << "Value for key "
        //           << "root_map_value is: " << v << std::endl;

        // Server server(config);
        // Server server_two(config_two);
        Runtime::instance().run();
        for (auto e : v_servers)
            delete e;
        for (auto e : v_configs)
            delete e;
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
