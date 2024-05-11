#include "TiniNode.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>

TiniNode::TiniNode() : _type(T_STRING), _stringValue(new std::string()) {}

TiniNode::TiniNode(std::string s) : _type(T_STRING), _stringValue(new std::string(s)) {}

TiniNode::TiniNode(TiniNodeType type) : _type(type)
{
    if (type == T_VECTOR)
        _vectorValue = new std::vector<TiniNode*>;
    if (type == T_MAP)
    {
        _mapValue = new std::map<std::string, TiniNode*>;
    }
    if (type == T_STRING)
        _stringValue = new std::string;
}

TiniNode& TiniNode::operator=(const TiniNode& other)
{
    if (this != &other)
    {
        if (_type == T_VECTOR)
            delete _vectorValue;
        else if (_type == T_MAP)
            delete _mapValue;
        else if (_type == T_STRING)
            delete _stringValue;
        _type = other._type;
        _vectorValue = nullptr;
        _mapValue = nullptr;
        _stringValue = nullptr;
        deepCopyChildren(other);
    }
    return *this;
}

TiniNode& TiniNode::operator=(const TiniNode* other)
{
    if (this != other && other != nullptr)
    {
        if (_type == T_VECTOR)
        {
            delete _vectorValue;
            _vectorValue = other->_vectorValue;
            _mapValue = nullptr;
            _stringValue = nullptr;
        }
        else if (_type == T_MAP)
        {
            delete _mapValue;
            _mapValue = other->_mapValue;
            _vectorValue = nullptr;
            _stringValue = nullptr;
        }
        else if (_type == T_STRING)
        {
            delete _stringValue;
            _stringValue = other->_stringValue;
            _vectorValue = nullptr;
            _mapValue = nullptr;
        }
        _type = other->_type;
    }
    return *this;
}

TiniNode::TiniNode(const TiniNode& other)
    : _type(other._type), _vectorValue(nullptr), _mapValue(nullptr), _stringValue(nullptr)
{
    deepCopyChildren(other);
}

void TiniNode::deepCopyChildren(const TiniNode& other)
{
    if (other._type == T_VECTOR)
    {
        _vectorValue = new std::vector<TiniNode*>();
        for (const TiniNode* element : *other._vectorValue)
        {
            _vectorValue->push_back(new TiniNode(*element));
        }
    }
    else if (other._type == T_MAP)
    {
        _mapValue = new std::map<std::string, TiniNode*>();
        for (const auto& pair : *other._mapValue)
        {
            _mapValue->insert({pair.first, new TiniNode(*pair.second)});
        }
    }
    else if (other._type == T_STRING)
    {
        _stringValue = new std::string(*other._stringValue);
    }
}

TiniNode TiniNode::operator[](size_t i) const
{
    if (_type != T_VECTOR || i >= this->getVectorValue().size())
        throw std::runtime_error(
            "TiniNode: Vector indexing operation error with key " + std::to_string(i)
        );
    return *(this->getVectorValue()[i]);
}

TiniNode& TiniNode::operator[](size_t i)
{
    if (_type != T_VECTOR || i >= this->getVectorValue().size())
        throw std::runtime_error(
            "TiniNode: Vector indexing operation error with key " + std::to_string(i)
        );
    return *(this->getVectorValue()[i]);
}

TiniNode TiniNode::operator[](const std::string& s) const
{
    TiniNode* ptr = nullptr;

    if (_type != T_MAP)
        throw std::runtime_error("TiniNode: Non map type for indexing operation with key " + s);
    ptr = this->getMapValue()[s];
    if (ptr == nullptr)
        throw std::runtime_error("TiniNode: Map indexing operation error wih key " + s);
    return *ptr;
}

TiniNode& TiniNode::operator[](const std::string& s)
{
    TiniNode* ptr = nullptr;

    if (_type != T_MAP)
        throw std::runtime_error("TiniNode: Non map type for indexing operation with key " + s);
    ptr = this->getMapValue()[s];
    if (ptr == nullptr)
        throw std::runtime_error("TiniNode: Map indexing operation error with key " + s);
    return *ptr;
}

TiniNode::~TiniNode()
{
    if (_type == T_VECTOR && _vectorValue)
    {
        for (auto v : *_vectorValue)
        {
            delete v;
        }
        delete _vectorValue;
        _vectorValue = nullptr;
    }
    else if (_type == T_MAP && _mapValue)
    {
        for (const auto& [key, val] : *_mapValue)
        {
            delete val;
        }
        delete _mapValue;
        _mapValue = nullptr;
    }
    else if (_type == T_STRING && _stringValue)
    {
        delete _stringValue;
        _stringValue = nullptr;
    }
}

TiniNode::TiniNode(TiniNode&& other) noexcept
    : _type(other._type), _vectorValue(nullptr), _mapValue(nullptr), _stringValue(nullptr)
{
    if (this == &other)
        return;

    switch (other._type)
    {
        case T_VECTOR:
            _vectorValue = other._vectorValue;
            other._vectorValue = nullptr;
            break;
        case T_MAP:
            _mapValue = other._mapValue;
            other._mapValue = nullptr;
            break;
        case T_STRING:
            _stringValue = other._stringValue;
            other._stringValue = nullptr;
            break;
    }
    other._type = T_STRING; // Resetting type of the moved-from object to avoid destructor issues
}

TiniNode& TiniNode::operator=(TiniNode&& other) noexcept
{
    if (this == &other)
        return *this;
    switch (_type)
    {
        case T_VECTOR:
            for (auto v : *_vectorValue)
            {
                delete v;
            }
            delete _vectorValue;
            break;
        case T_MAP:
            for (const auto& [key, val] : *_mapValue)
            {
                delete val;
            }
            delete _mapValue;
            break;
        case T_STRING:
            delete _stringValue;
            break;
    }
    if (other._type == T_STRING)
    {
        _type = T_STRING;
        _stringValue = other._stringValue;
        other._stringValue = nullptr;
    }
    if (other._type == T_VECTOR)
    {
        _type = T_VECTOR;
        _vectorValue = other._vectorValue;
        other._vectorValue = nullptr;
    }
    if (other._type == T_MAP)
    {
        _type = T_MAP;
        _mapValue = other._mapValue;
        other._mapValue = nullptr;
    }
    return *this;
}

TiniNode::TiniNodeType TiniNode::getType() const
{
    return _type;
}

TiniNode& TiniNode::fetchTiniNode(std::string key)
{
    if (_type == T_VECTOR)
    {
        if (!_vectorValue)
            throw std::runtime_error("TiniNode: fetchTiniNode: _vectorValue is null");
        for (size_t i = 0; i < _vectorValue->size(); ++i)
        {
            if ((*_vectorValue)[i]->getType() == T_MAP &&
                ((*_vectorValue)[i]->getMapValue().count(key) > 0))
                return *((*_vectorValue)[i]->getMapValue()[key]);
        }
    }
    else if (_type == T_MAP)
    {
        if (getMapValue().count(key) > 0)
            return *(getMapValue()[key]);
    }
    return *this;
}

std::vector<TiniNode*>& TiniNode::getVectorValue() const
{
    if (_type != T_VECTOR)
        throw std::runtime_error("TiniNode: getVectorValue: Tried to get non vector value as string"
        );
    if (_vectorValue == nullptr)
        throw std::runtime_error("TiniNode: getVectorValue: Attempted to get a null vector");
    return *_vectorValue;
}

std::map<std::string, TiniNode*>& TiniNode::getMapValue() const
{
    if (_type != T_MAP)
        throw std::runtime_error("TiniNode: getMapValue: Tried to get non map value as string");
    if (_mapValue == nullptr)
        throw std::runtime_error("TiniNode: getMapValue: Attempted to get a null map");
    return *_mapValue;
}

std::string& TiniNode::getStringValue() const
{
    if (_type != T_STRING)
        throw std::runtime_error("TiniNode: getStringValue: Tried to get non string value as string"
        );
    if (_stringValue == nullptr)
        throw std::runtime_error("TiniNode: getStringValue: Attempted to get a null string value");
    return *_stringValue;
}

void TiniNode::printContents(int depth, std::string name) const
{
    switch (_type)
    {
        case T_MAP:
            if (!_mapValue)
                throw std::runtime_error("TiniNode: printContents: _mapValue is null");
            for (const auto& [key, val] : *_mapValue)
            {
                if (val->getType() == T_STRING)
                {
                    for (int i = 0; i < depth; ++i)
                        std::cout << " ";
                    std::cout << "key: " << key << "   val: " << val->getStringValue() << std::endl;
                }
            }
            for (const auto& [key, val] : *_mapValue)
            {
                if (val->getType() == T_VECTOR)
                {
                    for (int i = 0; i < depth; ++i)
                        std::cout << " ";
                    std::cout << key << " vector values\n";
                    val->printContents(depth + 2, key);
                }
            }
            for (const auto& [key, val] : *_mapValue)
            {
                if (val->getType() == T_MAP)
                {
                    for (int i = 0; i < depth; ++i)
                        std::cout << " ";
                    std::cout << key << " map values\n";
                    val->printContents(depth + 2, key);
                }
            }
            break;
        case T_VECTOR:
            if (!_vectorValue)
                throw std::runtime_error("TiniNode: printContents: _vectorValue is null");
            for (unsigned long i = 0; i < _vectorValue->size(); ++i)
            {
                for (int j = 0; j < depth; ++j)
                    std::cout << " ";
                switch ((*_vectorValue)[i]->getType())
                {
                    case T_MAP:
                        std::cout << name << " map #" << i << " values\n";
                        (*_vectorValue)[i]->printContents(depth + 2, std::to_string(i));
                        break;
                    case T_VECTOR:
                        std::cout << name << " vector #" << i << " values\n";
                        (*_vectorValue)[i]->printContents(depth + 2, std::to_string(i));
                        break;
                    default:
                        throw std::runtime_error("TiniNode: printContents: Unexpected type");
                }
            }
            break;
        case T_STRING:
            if (!_stringValue)
                throw std::runtime_error("TiniNode: printContents: _stringValue is null");
            for (int i = 0; i < depth; ++i)
                std::cout << " ";
            std::cout << "string value: " << *_stringValue;
            break;
    }
}

void TiniNode::setFirstValue(std::pair<std::string, TiniNode*> pair)
{
    _firstMapValue = pair;
}

std::optional<std::pair<std::string, TiniNode*>> TiniNode::getFirstValue() const
{
    return _firstMapValue;
}

#ifdef TEST

#include "testutils.hpp"

void TiniNodeTest::deepcopy_test()
{
    BEGIN
    TiniNode* root = new TiniNode(TiniNode::T_MAP);
    auto&     root_map = root->getMapValue();

    root_map.insert(std::make_pair(std::string("nested_map"), new TiniNode(TiniNode::T_MAP)));
    root_map.insert(std::make_pair(std::string("vector"), new TiniNode(TiniNode::T_VECTOR)));
    root_map.insert(std::make_pair(std::string("string"), new TiniNode(TiniNode::T_STRING)));

    TiniNode* nested = root_map["nested_map"];
    auto&     nested_map = nested->getMapValue();
    nested_map["first"] = new TiniNode(TiniNode::T_STRING);
    nested_map["second"] = new TiniNode(TiniNode::T_STRING);
    nested_map["third"] = new TiniNode(TiniNode::T_STRING);
    nested_map["twodeep"] = new TiniNode(TiniNode::T_MAP);

    TiniNode* twodeep = nested_map["twodeep"];
    auto&     twodeep_map = twodeep->getMapValue();
    twodeep_map["first"] = new TiniNode(TiniNode::T_STRING);
    twodeep_map["second"] = new TiniNode(TiniNode::T_STRING);
    twodeep_map["third"] = new TiniNode(TiniNode::T_STRING);

    TiniNode* rootcopy = new TiniNode();
    *rootcopy = *root;

    auto& rootcopymap = rootcopy->getMapValue();

    EXPECT(rootcopymap["nested_map"] != root_map["nested_map"]);
    EXPECT(rootcopymap["vector"] != root_map["vector"]);
    EXPECT(rootcopymap["string"] != root_map["string"]);
    EXPECT(rootcopymap["string"]->getType() == TiniNode::T_STRING);
    EXPECT(rootcopymap["vector"]->getType() == TiniNode::T_VECTOR);
    EXPECT(rootcopymap["nested_map"]->getType() == TiniNode::T_MAP);

    TiniNode* rootcopy_nested = rootcopymap["nested_map"];
    auto&     rootcopy_nested_map = rootcopy_nested->getMapValue();

    TiniNode* rootcopy_twodeep = rootcopy_nested_map["twodeep"];
    auto&     rootcopy_twodeep_map = rootcopy_twodeep->getMapValue();
    EXPECT(rootcopy_twodeep_map["first"] != twodeep_map["first"]);
    EXPECT(rootcopy_twodeep_map["second"] != twodeep_map["second"]);
    EXPECT(rootcopy_twodeep_map["third"] != twodeep_map["third"]);

    EXPECT(rootcopy_nested != nullptr);
    EXPECT(rootcopy_nested != nested);

    delete root;
    delete rootcopy;
    END
}

#endif
