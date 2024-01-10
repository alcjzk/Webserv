#include "TiniNode.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

TiniNode::TiniNode() : _type(T_STRING), _stringValue(new std::string()) {}

TiniNode::TiniNode(std::string s)
    : _type(T_STRING), _stringValue(new std::string(s)) {}

TiniNode::TiniNode(TiniNodeType type) : _type(type) {
  if (type == T_VECTOR)
    _vectorValue = new std::vector<TiniNode *>;
  if (type == T_MAP)
    _mapValue = new std::map<std::string, TiniNode *>;
  if (type == T_STRING)
    _stringValue = new std::string;
}

TiniNode &TiniNode::operator=(const TiniNode &other) {
  if (this != &other) {
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

TiniNode::TiniNode(const TiniNode &other)
    : _type(other._type), _vectorValue(nullptr), _mapValue(nullptr),
      _stringValue(nullptr) {
  deepCopyChildren(other);
}

void TiniNode::deepCopyChildren(const TiniNode &other) {
  if (other._type == T_VECTOR) {
    _vectorValue = new std::vector<TiniNode *>();
    for (const TiniNode *element : *other._vectorValue) {
      _vectorValue->push_back(new TiniNode(*element));
    }
  } else if (other._type == T_MAP) {
    _mapValue = new std::map<std::string, TiniNode *>();
    for (const auto &pair : *other._mapValue) {
      _mapValue->insert({pair.first, new TiniNode(*pair.second)});
    }
  } else if (other._type == T_STRING) {
    _stringValue = new std::string(*other._stringValue);
  }
}

TiniNode TiniNode::operator[](size_t i) const {
  if (_type != T_VECTOR || i >= this->getVectorValue().size())
    throw std::runtime_error(
        "TiniNode: Vector indexing operation error with key " +
        std::to_string(i));
  return *(this->getVectorValue()[i]);
}

TiniNode &TiniNode::operator[](size_t i) {
  if (_type != T_VECTOR || i >= this->getVectorValue().size())
    throw std::runtime_error(
        "TiniNode: Vector indexing operation error with key " +
        std::to_string(i));
  return *(this->getVectorValue()[i]);
}

TiniNode TiniNode::operator[](const std::string &s) const {
  TiniNode *ptr = nullptr;

  if (_type != T_MAP)
    throw std::runtime_error(
        "TiniNode: Non map type for indexing operation with key " + s);
  ptr = this->getMapValue()[s];
  if (ptr == nullptr)
    throw std::runtime_error("TiniNode: Map indexing operation error wih key " +
                             s);
  return *ptr;
}

TiniNode &TiniNode::operator[](const std::string &s) {
  TiniNode *ptr = nullptr;

  if (_type != T_MAP)
    throw std::runtime_error(
        "TiniNode: Non map type for indexing operation with key " + s);
  ptr = this->getMapValue()[s];
  if (ptr == nullptr)
    throw std::runtime_error(
        "TiniNode: Map indexing operation error with key " + s);
  return *ptr;
}

TiniNode::~TiniNode() {
  if (_type == T_VECTOR) {
    for (auto v : *_vectorValue) {
      delete v;
      v = nullptr;
    }
    _vectorValue->clear();
    delete _vectorValue;
    _vectorValue = nullptr;
  } else if (_type == T_MAP) {
    for (auto const &[key, val] : *_mapValue) {
      delete val;
    }
    _mapValue->clear();
    delete _mapValue;
    _mapValue = nullptr;
  } else if (_type == T_STRING) {
    delete _stringValue;
    _stringValue = nullptr;
  }
}

TiniNode::TiniNode(TiniNode&& other)
{
    if (this == &other)
        return;
    switch (other._type)
    {
        case T_VECTOR:
            _type = T_VECTOR;
            _vectorValue = other._vectorValue;
            other._vectorValue = nullptr;
            break;
        case T_MAP:
            _type = T_MAP;
            _mapValue = other._mapValue;
            other._mapValue = nullptr;
            break;
        case T_STRING:
            _type = T_STRING;
            _stringValue = other._stringValue;
            other._stringValue = nullptr;
            break;
    }
}

TiniNode& TiniNode::operator=(TiniNode&& other)
{
    if (this == &other)
        return *this;
    switch (_type)
    {
        case T_VECTOR:
            if (_vectorValue)
                _vectorValue->clear();
            delete _vectorValue;
            break;
        case T_MAP:
            if (_mapValue)
                _mapValue->clear();
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

TiniNode::TiniNodeType TiniNode::getType() const { return _type; }

TiniNode &TiniNode::fetchTiniNode(std::string key) {
  if (_type == T_VECTOR) {
    for (size_t i = 0; i < _vectorValue->size(); ++i) {
      if ((*_vectorValue)[i]->getType() == T_MAP &&
          ((*_vectorValue)[i]->getMapValue().count(key) > 0))
        return *((*_vectorValue)[i]->getMapValue()[key]);
    }
  } else if (_type == T_MAP) {
    if (getMapValue().count(key) > 0)
      return *(getMapValue()[key]);
  }
  return *this;
}

std::vector<TiniNode *> &TiniNode::getVectorValue() const {
  if (_type != T_VECTOR)
    throw std::runtime_error(
        "TiniNode: getVectorValue: Tried to get non vector value as string");
  return *_vectorValue;
}

std::map<std::string, TiniNode *> &TiniNode::getMapValue() const {
  if (_type != T_MAP)
    throw std::runtime_error(
        "TiniNode: getMapValue: Tried to get non map value as string");
  return *_mapValue;
}

std::string &TiniNode::getStringValue() const {
  if (_type != T_STRING)
    throw std::runtime_error(
        "TiniNode: getStringValue: Tried to get non string value as string");
  return *_stringValue;
}

void TiniNode::printContents(int depth, std::string name) const {
  switch (_type) {
  case T_MAP:
    for (auto const &[key, val] : *_mapValue) {
      if (val->getType() == T_STRING) {
        for (int i = 0; i < depth; ++i)
          std::cout << " ";
        std::cout << "key: " << key << "   val: " << val->getStringValue()
                  << std::endl;
      }
    }
    for (auto const &[key, val] : *_mapValue) {
      if (val->getType() == T_VECTOR) {
        for (int i = 0; i < depth; ++i)
          std::cout << " ";
        std::cout << key << " vector values\n";
        val->printContents(depth + 2, key);
      }
    }
    for (auto const &[key, val] : *_mapValue) {
      if (val->getType() == T_MAP) {
        for (int i = 0; i < depth; ++i)
          std::cout << " ";
        std::cout << key << " map values\n";
        val->printContents(depth + 2, key);
      }
    }
    break;
  case T_VECTOR:
    for (unsigned long i = 0; i < _vectorValue->size(); ++i) {
      for (int j = 0; j < depth; ++j)
        std::cout << " ";
      switch ((*_vectorValue)[i]->getType()) {
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
    for (int i = 0; i < depth; ++i)
      std::cout << " ";
    std::cout << "string value: " << *_stringValue;
    break;
  }
}
