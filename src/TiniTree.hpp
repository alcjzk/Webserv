#include "TiniNode.hpp"
#include "TiniUtils.hpp"
#include "TiniValidator.hpp"
#include <vector>

#ifndef TINITREE_HPP
#define TINITREE_HPP

class TiniTree {
private:
  int _ctx;
  TiniNode *_current;
  std::string _filename;
  TiniValidator _validator;
  std::vector<std::string> _split_input;

public:
  TiniNode *_root;
  TiniTree();
  ~TiniTree();
  TiniTree(std::string config_location);
  TiniTree(const TiniTree &other) = delete;
  TiniTree &operator=(const TiniTree &other) = delete;
  TiniTree(TiniTree&& other) = delete;
  TiniTree& operator=(TiniTree&& other) = delete;

  void constructTree();
  int contextSwitch(std::string ctx, int row);
  int valueInsertion(std::string act, int row);
  TiniNode &getRoot();
};

#endif
