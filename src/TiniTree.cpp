#include "TiniTree.hpp"
#include "TiniUtils.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

TiniTree::TiniTree() : _current(nullptr), _root(nullptr)
{
    try
    {
        std::ifstream     f(_CONFIG_NAME);
        std::stringstream ss;

        ss << f.rdbuf();
        std::string input_string = ss.str();
        _split_input = tiniutils::removeSpaces(tiniutils::split(input_string, "\n"));
    }
    catch (std::exception& e)
    {
        std::cerr << "TiniTree: FATAL: " << e.what() << std::endl;
        return;
    }

    constructTree();
    if (!_root)
        throw std::runtime_error("TiniTree: FATAL ");
}

TiniTree::TiniTree(std::string config_location) : _current(nullptr), _root(nullptr)
{
    try
    {
        std::ifstream     f(config_location);
        std::stringstream ss;

        ss << f.rdbuf();
        std::string input_string = ss.str();
        _split_input = tiniutils::removeSpaces(tiniutils::split(input_string, "\n"));
    }
    catch (std::exception& e)
    {
        std::cerr << "TiniTree: FATAL: " << e.what() << std::endl;
        return;
    }

    constructTree();
    if (!_root)
        throw std::runtime_error("TiniTree: FATAL ");
}

TiniTree::~TiniTree()
{
    if (_root)
        delete _root;
}

int TiniTree::contextSwitch(std::string ctx, int row)
{
    std::string              processed;
    std::vector<std::string> map_ops;
    bool                     vec = false;
    TiniNode*                tmp = nullptr;

    if (ctx[1] == '[')
    {
        processed = ctx.substr(2, ctx.length() - 4);
        vec = true;
    }
    else
        processed = ctx.substr(1, ctx.length() - 2);
    map_ops = tiniutils::split(processed, std::string("."), '\\');
    _current = _root;
    for (size_t i = 0; i < map_ops.size(); ++i)
    {
        if (_current->getType() == TiniNode::T_MAP || _current->getType() == TiniNode::T_VECTOR)
        {
            if (i == map_ops.size() - 1)
            {
                tmp = &_current->fetchTiniNode(map_ops[i]);
                if (tmp == _current)
                {
                    if (vec)
                    {
                        if (_current->getType() == TiniNode::T_VECTOR)
                            _current->getVectorValue().push_back(new TiniNode(TiniNode::T_VECTOR));
                        else
                            _current->getMapValue()[map_ops[i]] = new TiniNode(TiniNode::T_VECTOR);
                    }
                    else
                    {
                        if (_current->getType() == TiniNode::T_VECTOR)
                            _current->getVectorValue().push_back(new TiniNode(TiniNode::T_MAP));
                        else
                        {
                            _current->getMapValue()[map_ops[i]] = new TiniNode(TiniNode::T_MAP);
                            trySetFirst(_current, map_ops[i]);
                        }
                    }
                    tmp = &_current->fetchTiniNode(map_ops[i]);
                }
                if (vec)
                {
                    if (tmp->getType() != TiniNode::T_VECTOR)
                        throw std::runtime_error("TiniTree: contextSwitch: Expected vector value");
                    tmp->getVectorValue().push_back(new TiniNode(TiniNode::T_MAP));
                    tmp = tmp->getVectorValue().back();
                }
                _current = tmp;
            }
            else
            {
                tmp = &_current->fetchTiniNode(map_ops[i]);
                if (tmp == _current)
                {
                    if (_current->getType() == TiniNode::T_VECTOR)
                        _current->getVectorValue().push_back(new TiniNode(TiniNode::T_MAP));
                    else
                    {
                        _current->getMapValue()[map_ops[i]] = new TiniNode(TiniNode::T_MAP);
                        trySetFirst(_current, map_ops[i]);
                    }
                    tmp = &_current->fetchTiniNode(map_ops[i]);
                }
                _current = tmp;
            }
        }
        else
            throw std::runtime_error("TiniTree: contextSwitch: Unexpected lone string in tree");
    }
    return (0);
}

int TiniTree::valueInsertion(std::string act, int row)
{
    std::vector<std::string> pair = tiniutils::split(act, "=");
    switch (_current->getType())
    {
        case TiniNode::T_VECTOR:
            throw std::runtime_error("TiniTree: valueInsertion: Vector cannot act as endpoint");
            break;
        case TiniNode::T_MAP:
            if (_current->getMapValue().count(pair[0]))
                throw std::runtime_error("TiniTree: valueInsertion: Duplicate key error!");
            _current->getMapValue()[pair[0]] = new TiniNode(pair[1]);
            break;
        case TiniNode::T_STRING:
            throw std::runtime_error("TiniTree: valueInsertion: String cannot act as endpoint");
            break;
        default:
            throw std::runtime_error("TiniTree: valueInsertion: Unknown action value");
    }
    return (0);
}

void TiniTree::constructTree()
{
    if (_validator.validateConfig(_split_input) == -1)
        throw std::runtime_error("TiniTree: constructTree: FATAL");

    try
    {
        _root = new TiniNode(TiniNode::T_MAP);
    }
    catch (const std::bad_alloc& e)
    {
        std::cerr << e.what() << std::endl;
        throw std::runtime_error("TiniTree: constructTree: FATAL");
    }
    _current = _root;

    for (unsigned long i = 0; i < _split_input.size(); ++i)
    {
        try
        {
            switch (tiniutils::match_input(_split_input[i]))
            {
                case TiniNode::O_CTX:
                    contextSwitch(_split_input[i], i);
                    break;
                case TiniNode::O_INS:
                    valueInsertion(_split_input[i], i);
                    break;
                default:
                    break;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "TiniTree: constructTree: " << e.what() << std::endl;
            delete _root;
            _root = nullptr;
            throw std::runtime_error("TiniTree: constructTree: FATAL");
        }
    }
}

TiniNode& TiniTree::getRoot()
{
    return *_root;
}

void TiniTree::trySetFirst(TiniNode* current, std::string key)
{
    if (!current->getFirstValue().has_value() && key[0] != '/')
        current->setFirstValue(*current->getMapValue().find(key));
}
