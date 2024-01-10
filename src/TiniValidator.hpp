#ifndef TINI_VALIDATOR_HPP
#define TINI_VALIDATOR_HPP
#include "TiniNode.hpp"
#include "TiniUtils.hpp"

class TiniValidator
{
    private:
        std::map<int, std::vector<int>> _context_transitions = {
            {TiniNode::S_OBRACK, {TiniNode::S_OBRACK, TiniNode::S_ALPHA}},
            {TiniNode::S_ALPHA, {TiniNode::S_ALPHA, TiniNode::S_DOT, TiniNode::S_CBRACK}},
            {TiniNode::S_DOT, {TiniNode::S_ALPHA}},
            {TiniNode::S_CBRACK, {TiniNode::S_CBRACK}}};
        std::map<int, std::vector<int>> _value_transitions = {
            {TiniNode::S_ISPRINT, {TiniNode::S_ISPRINT, TiniNode::S_EQ}},
            {TiniNode::S_EQ, {TiniNode::S_ISPRINT}}};
        std::map<int, std::vector<int>>::iterator _map_idx;
        int                                       _cur_state = TiniNode::S_NONE;
        int                                       _next_state = TiniNode::S_NONE;
        int                                       _errc = 0;

    public:
        TiniValidator() = default;
        ~TiniValidator() = default;
        TiniValidator(const TiniValidator& other) = default;
        TiniValidator& operator=(const TiniValidator& other) = default;
        int            validateConfig(std::vector<std::string>& split_input);
        int            validateContext(std::string ctx, int row);
        int            validateValue(std::string act, int row);
};

#endif
