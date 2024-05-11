#include <iostream>
#include "TiniValidator.hpp"
#include "TiniUtils.hpp"
#include <algorithm>

int TiniValidator::validateContext(std::string ctx, int row)
{
    int                                       balance = 0;
    std::map<int, std::vector<int>>::iterator map_idx;

    _cur_state = TiniNode::S_NONE;
    _next_state = TiniNode::S_NONE;
    for (std::string::iterator it = ctx.begin(); it != ctx.end(); ++it)
    {
        if (it == ctx.begin())
            _cur_state = tiniutils::chr_to_ctx_state(*it, '\0');
        else
            _cur_state = tiniutils::chr_to_ctx_state(*it, *(it - 1));
        _next_state = tiniutils::chr_to_ctx_state(*(it + 1), *it);
        map_idx = _context_transitions.find(_cur_state);
        if (it < ctx.end() - 1)
        {
            if (map_idx == _context_transitions.end())
            {
                tiniutils::printErr(TiniNode::E_UNEXP, it - ctx.begin() + 1, row, ctx);
                return (-1);
            }
            if (std::any_of(
                    _context_transitions[_cur_state].begin(),
                    _context_transitions[_cur_state].end(),
                    [&](int correct_transition) { return correct_transition == _next_state; }
                ))
                break;
            tiniutils::printErr(TiniNode::E_UNEXP, it - ctx.begin() + 1, row, ctx);
            return (-1);
        }
        switch (_cur_state)
        {
            case TiniNode::S_OBRACK:
            {
                ++balance;
                if (balance > 2)
                {
                    tiniutils::printErr(TiniNode::E_BRACKC_HIGH, it - ctx.begin(), row, ctx);
                    return (-1);
                }
                break;
            }
            case TiniNode::S_CBRACK:
            {
                --balance;
                if (balance < 0)
                {
                    tiniutils::printErr(TiniNode::E_BRACKC_LOW, it - ctx.begin(), row, ctx);
                    return (-1);
                }
                break;
            }
            default:
                break;
        }
    }
    if (balance != 0)
    {
        tiniutils::printErr(TiniNode::E_BRACKBAL, ctx.size() - 1, row, ctx);
        return (-1);
    }
    return (0);
}

int TiniValidator::validateValue(std::string act, int row)
{
    bool                                      eq_visited = false;
    std::map<int, std::vector<int>>::iterator map_idx;

    _cur_state = TiniNode::S_NONE;
    _next_state = TiniNode::S_NONE;
    for (std::string::iterator it = act.begin(); it != act.end(); ++it)
    {
        _cur_state = tiniutils::chr_to_act_state(*it);
        _next_state = tiniutils::chr_to_act_state(*(it + 1));
        map_idx = _value_transitions.find(_cur_state);
        if (it < act.end() - 1)
        {
            if (map_idx == _value_transitions.end())
            {
                tiniutils::printErr(TiniNode::E_UNEXP, it - act.begin() + 1, row, act);
                return (-1);
            }
            if (!std::any_of(
                    _value_transitions[_cur_state].begin(), _value_transitions[_cur_state].end(),
                    [&](int correct_transition) { return correct_transition == _next_state; }
                ))
            {
                tiniutils::printErr(TiniNode::E_UNEXP, it - act.begin() + 1, row, act);
                return (-1);
            }
        }
        if (_cur_state == TiniNode::S_EQ)
        {
            if (eq_visited)
            {
                tiniutils::printErr(TiniNode::E_EQCOUNT, act.size() - 1, row, act);
                return (-1);
            }
            eq_visited = true;
        }
    }
    if (!eq_visited)
    {
        tiniutils::printErr(TiniNode::E_NOEQ, act.size() - 1, row, act);
        return (-1);
    }
    return (0);
}

int TiniValidator::validateConfig(const std::vector<std::string>& split_input)
{
    _errc = 0;

    for (unsigned long i = 0; i < split_input.size(); ++i)
    {
        switch (tiniutils::match_input(split_input[i]))
        {
            case TiniNode::O_CTX:
                if (validateContext(split_input[i], i) == -1)
                    ++_errc;
                break;
            case TiniNode::O_INS:
                if (validateValue(split_input[i], i) == -1)
                    ++_errc;
                break;
            default:
                break;
        }
    }
    if (_errc)
    {
        std::cerr << "Found a total of " << _errc << " errors in the configuration!" << std::endl;
        return (-1);
    }
    return (0);
}
