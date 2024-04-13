#include "Config.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "TiniTree.hpp"
#include "Runtime.hpp"
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <set>
#include <assert.h>
#include <memory>

int main()
{
    std::vector<std::unique_ptr<Server>> v_servers;
    std::set<std::string>                opened_ports;

    signal(SIGPIPE, SIG_IGN);
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
            Config cfg(val->getMapValue(), root.getMapValue(), val->getFirstValue().value());
            if (opened_ports.find(cfg.port()) != opened_ports.end())
                throw std::runtime_error("Port already defined");
            opened_ports.insert(cfg.port());
            v_servers.push_back(std::make_unique<Server>(std::move(cfg)));
        }
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
    catch (...)
    {
        ERR("STATIC TEXT");
        return EXIT_FAILURE;
    }

#ifdef TEST
    system("leaks debug");
#endif

    return EXIT_SUCCESS;
}
