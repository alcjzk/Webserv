#include "TiniNode.hpp"
#include "TiniUtils.hpp"
#include "TiniValidator.hpp"

#include <vector>

#ifndef TINITREE_HPP
#define TINITREE_HPP

class TiniTree
{
    private:
        static constexpr const char* _CONFIG_NAME = "config.tini";
        TiniNode*                    _current;
        TiniNode*                    _root;
        TiniValidator                _validator;
        std::vector<std::string>     _split_input;

    public:
        TiniTree();
        ~TiniTree();
        TiniTree(std::string config_location);
        TiniTree(const TiniTree& other) = delete;
        TiniTree& operator=(const TiniTree& other) = delete;
        TiniTree(TiniTree&& other) = delete;
        TiniTree& operator=(TiniTree&& other) = delete;

        void      constructTree();
        int       contextSwitch(std::string ctx, int row);
        int       valueInsertion(std::string act, int row);
        void      trySetFirst();
        void      trySetFirst(TiniNode* current, std::string key);
        TiniNode& getRoot();
};

#endif
