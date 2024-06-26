#include "Config.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "TiniTree.hpp"
#include "Runtime.hpp"
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <set>
#include <signal.h>
#include <assert.h>
#include <memory>

int main(int argc, char* argv[])
{
    std::vector<std::unique_ptr<Server>> v_servers;
    std::set<std::string>                opened_ports;
    std::unique_ptr<TiniTree>            tree;

    signal(SIGPIPE, SIG_IGN);
    try
    {
        if (argc > 1)
            tree = std::make_unique<TiniTree>(std::string(argv[1]));
        else
        {
            INFO("configuration location not given, defaulting to config.tini");
            tree = std::make_unique<TiniTree>();
        }
        const TiniNode& root = tree->getRoot();
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
