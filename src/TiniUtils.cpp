#include <string>
#include <vector>
#include "TiniNode.hpp"
#include "TiniUtils.hpp"

std::vector<std::string> split(std::string s, std::string d)
{
    std::vector<std::string> ret;
    std::string tmp;
    size_t end = 0;
    size_t start = 0;
    size_t d_len = d.length();

    do
    {
        end = s.find(d, start);
        tmp = s.substr (start, end - start);
        start = end + d_len;
        ret.push_back(tmp);
    } while (end != std::string::npos);
    return ret;
}

int match_input(std::string input)
{
    if (input.size())
    {
        if (input.at(0) == '[')
            return (TiniNode::O_CTX);
        return (TiniNode::O_INS);
    }
    return (-1);
}

int chr_to_ctx_state(char c)
{
    if (isalpha(c))
        return TiniNode::S_ALPHA;
    switch (c)
    {
        case '[':
            return TiniNode::S_OBRACK;
        case '.':
            return TiniNode::S_DOT;
        case ']':
            return TiniNode::S_CBRACK;
    }
    return (-1);
}

int chr_to_act_state(char c)
{
    if (c == '=')
        return TiniNode::S_EQ;
    if (isprint(c))
        return TiniNode::S_ISPRINT;
    return (-1);
}

std::vector<std::string> removeSpaces(std::vector<std::string> split_input)
{
    std::vector<std::string> no_spaces;

    for (auto s : split_input)
    {
        std::string tmp;
        for (auto c : s)
        {
            if (!(isspace(c)))
                tmp.push_back(c);
        }
        no_spaces.push_back(tmp);
    }
    return no_spaces;
}

void    printErr(int err_type, int col, int row, std::string line)
{
    for (int i = 0; i < col; ++i)
        std::cerr << " ";
    std::cerr << "v" << std::endl;
    std::cerr << line << std::endl;
    switch (err_type)
    {
        case TiniNode::E_UNEXP:
            std::cerr << "Error: Unexpected Token";
            break;
        case TiniNode::E_BRACKC_HIGH:
            std::cerr << "Error: Too Many Brackets";
            break;
        case TiniNode::E_BRACKC_LOW:
            std::cerr << "Error: Extra Bracket";
            break;
        case TiniNode::E_BRACKBAL:
            std::cerr << "Error: Brackets Imbalanced";
            break;
        case TiniNode::E_EQCOUNT:
            std::cerr << "Error: Too Many Assignments";
            break;
        case TiniNode::E_NOEQ:
            std::cerr << "Error: Only Key Provided";
            break;
    }
    std::cerr << " at R: " << row + 1 << " C: " << col + 1 << "\n" << std::endl;
}
