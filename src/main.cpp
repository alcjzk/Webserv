#include "Config.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "TiniTree.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <set>
#include <assert.h>

void cleanup(std::vector<Server*> s, std::vector<Config*> c)
{
    for (auto e : s)
        delete e;
    for (auto e : c)
        delete e;
}

int main()
{
    std::vector<Server*>  v_servers;
    std::vector<Config*>  v_configs;
    std::set<std::string> opened_ports;
    try
    {
        TiniTree        tree;
        const TiniNode& root = tree.getRoot();
        const TiniNode* servers = root.getMapValue()["servers"];

        if (!servers)
            throw std::runtime_error("\"servers\" not found in the root map");
        for (const auto& [key, val] : servers->getMapValue())
        {
            std::optional<std::pair<std::string, TiniNode*>> first_pair = val->getFirstValue();
            assert(first_pair.has_value());
            Config* cfg =
                new Config(val->getMapValue(), root.getMapValue(), val->getFirstValue().value());

            if (opened_ports.find(cfg->port()) != opened_ports.end())
            {
                delete cfg;
                throw std::runtime_error("Port already defined");
            }
            opened_ports.insert(cfg->port());
            v_configs.push_back(cfg);
            v_servers.push_back(new Server(*v_configs.back()));
        }
        Runtime::instance().run();
        cleanup(v_servers, v_configs);
    }
    catch (const char* e)
    {
        ERR(e);
        cleanup(v_servers, v_configs);
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        ERR(e.what());
        cleanup(v_servers, v_configs);
        return EXIT_FAILURE;
    }

#ifdef TEST
    system("leaks debug");
#endif

    return EXIT_SUCCESS;
}
