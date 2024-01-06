#include <map>
#include <vector>

#ifndef TINI_NODE_H
#define TINI_NODE_H

class TiniNode {
public:
  enum TiniNodeType {
    T_VECTOR,
    T_MAP,
    T_STRING,
  };
  enum e_tini_opcode {
    O_CTX,
    O_INS,
  };

  enum e_tini_ctx_state {
    S_NONE,
    S_OBRACK,
    S_ALPHA,
    S_ISPRINT,
    S_EQ,
    S_DOT,
    S_CBRACK,
  };
  enum e_tini_errcodes {
    E_UNEXP,
    E_BRACKC_LOW,
    E_BRACKC_HIGH,
    E_BRACKBAL,
    E_EQCOUNT,
    E_NOEQ,
  };

private:
  TiniNodeType _type;
  // Could be union or some sort of variant type, but feel like the dynamic
  // casting is not worth the space savings here
  std::vector<TiniNode *> *_vectorValue;
  std::map<std::string, TiniNode *> *_mapValue;
  std::string *_stringValue;

public:
  TiniNode();
  ~TiniNode();
  TiniNode(TiniNodeType type);
  TiniNode(const TiniNode &other);
  TiniNode &operator=(const TiniNode &other);
  TiniNode(std::string s);
  TiniNode operator[](int i) const;
  TiniNode &operator[](int i);
  TiniNode operator[](const std::string &s) const;
  TiniNode &operator[](const std::string &s);
  TiniNode &fetchTiniNode(std::string key);
  TiniNodeType getType() const;
  std::vector<TiniNode *> &getVectorValue() const;
  std::map<std::string, TiniNode *> &getMapValue() const;
  std::string &getStringValue() const;
  void printContents(int depth, std::string name) const;
};

#endif
