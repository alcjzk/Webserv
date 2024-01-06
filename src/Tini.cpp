#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <sys/_types/_size_t.h>
#include <utility>
#include <vector>
#include <map>
#include <ctype.h>

enum e_ctx_state
{
    S_NONE,
    S_OBRACK,
    S_ALPHA,
    S_ISPRINT,
    S_EQ,
    S_DOT,
    S_CBRACK,
};

enum e_parse_err
{
    E_UNEXP,
    E_BRACKC_LOW,
    E_BRACKC_HIGH,
    E_BRACKBAL,
    E_EQCOUNT,
    E_NOEQ,
};

class Node
{
    public:
        enum NodeType
        {
            VECTOR,
            MAP,
            STRING
        };
    private:
        NodeType _type;
        std::vector<Node*>* _vectorValue = nullptr;
        std::map<std::string, Node*>* _mapValue = nullptr;
        std::string* _stringValue = nullptr;
    public:
        Node() : _type(STRING), _stringValue(new std::string()) { }
        Node(std::string s) : _type(STRING), _stringValue(new std::string(s)) { }
        Node(NodeType type) : _type(type)
        {
            if (type == VECTOR)
                _vectorValue = new std::vector<Node*>;
            if (type == MAP)
                _mapValue = new std::map<std::string, Node*>;
            if (type == STRING)
                _stringValue = new std::string;
        }
        Node(const Node& other) : _type(other._type)
        {
            if (other._type == VECTOR)
                _vectorValue = new std::vector<Node*>(*other._vectorValue);
            else if (other._type == MAP)
                _mapValue = new std::map<std::string, Node*>(*other._mapValue);
            else if (other._type == STRING)
                _stringValue = new std::string(*other._stringValue);
        }
        Node& operator=(const Node& other)
        {
            if (this != &other)
            {
                if (_type == VECTOR)
                    delete _vectorValue;
                else if (_type == MAP)
                    delete _mapValue;
                else if (_type == STRING)
                    delete _stringValue;
                _type = other._type;
                _vectorValue = nullptr;
                _mapValue = nullptr;
                _stringValue = nullptr;
                if (other._type == VECTOR)
                    _vectorValue = new std::vector<Node*>(*other._vectorValue);
                else if (other._type == MAP)
                    _mapValue = new std::map<std::string, Node*>(*other._mapValue);
                else if (other._type == STRING)
                    _stringValue = new std::string(*other._stringValue);
            }
            return *this;
        }
        ~Node()
        {
            if (_type == VECTOR)
            {
                for (auto v : *_vectorValue)
                    delete v;
                delete _vectorValue;
            }
            else if (_type == MAP)
            {
                for (auto const& [key, val] : *_mapValue)
                    delete val;
                delete _mapValue;
            }
            else if (_type == STRING)
                delete _stringValue;
        }
        NodeType getType() const
        {
            return _type;
        }
        std::vector<Node*>& getVectorValue() const
        {
            if (_type != VECTOR)
                throw "Node: getVectorValue: Tried to get non vector value as string";
            return *_vectorValue;
        }
        std::map<std::string, Node*>& getMapValue() const
        {
            if (_type != MAP)
                throw "Node: getMapValue: Tried to get non map value as string";
            return *_mapValue;
        }
        std::string& getStringValue() const
        {
            if (_type != STRING)
                throw "Node: getStringValue: Tried to get non string value as string";
            return *_stringValue;
        }
        Node& fetch(std::string key)
        {
            if (_type == VECTOR)
            {
                for (size_t i = 0; i < _vectorValue->size(); ++i)
                {
                    if ((*_vectorValue)[i]->getType() == MAP && ((*_vectorValue)[i]->getMapValue().count(key) > 0))
                            return *((*_vectorValue)[i]->getMapValue()[key]);
                }
            }
            else if (_type == MAP)
            {
                if (getMapValue().count(key) > 0)
                    return *(getMapValue()[key]);
            }
            return *this;
        }
        void    print_contents(int depth, std::string name)
        {
            switch (_type)
            {
                case MAP:
                    for (auto const& [key, val] : *_mapValue)
                    {
                        if (val->getType() == STRING)
                        {
                            for (size_t i = 0; i < depth; ++i)
                                std::cout << " ";
                            std::cout << "key: " << key << "   val: " << val->getStringValue() << std::endl;
                        }
                    }
                    for (auto const& [key, val] : *_mapValue)
                    {
                        if (val->getType() == VECTOR)
                        {
                            for (size_t i = 0; i < depth; ++i)
                                std::cout << " ";
                            std::cout << key << " vector values\n";
                            val->print_contents(depth + 2, key);
                        }
                    }
                    for (auto const& [key, val] : *_mapValue)
                    {
                        if (val->getType() == MAP)
                        {
                                for (size_t i = 0; i < depth; ++i)
                                    std::cout << " ";
                                std::cout << key << " map values\n";
                                val->print_contents(depth + 2, key);
                        }
                    }
                    break;
                case VECTOR:
                    for (size_t i = 0; i < _vectorValue->size(); ++i)
                    {
                        for (size_t j = 0; j < depth; ++j)
                            std::cout << " ";
                        switch ((*_vectorValue)[i]->getType())
                        {
                            case MAP:
                                std::cout << name << " map #" << i << " values\n";
                                (*_vectorValue)[i]->print_contents(depth + 2, std::to_string(i));
                                break;
                            case VECTOR:
                                std::cout << name << " vector #" << i << " values\n";
                                (*_vectorValue)[i]->print_contents(depth + 2, std::to_string(i));
                                break;
                            default:
                                throw "Node: print_contents: Unexpected type";
                        }
                    }
                    break;
                case STRING:
                    for (size_t i = 0; i < depth; ++i)
                        std::cout << " ";
                    std::cout << "string value: " << *_stringValue;
                    break;
            }
        }
};


struct ctx_state
{
    int         ctx;
    Node  *root;
    Node  *current;
};

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
            return (1);
        return (2);
    }
    return (0);
}

int chr_to_ctx_state(char c)
{
    if (isalpha(c))
        return S_ALPHA;
    switch (c)
    {
        case '[':
            return S_OBRACK;
        case '.':
            return S_DOT;
        case ']':
            return S_CBRACK;
    }
    return (-1);
}

int chr_to_act_state(char c)
{
    if (c == '=')
        return S_EQ;
    if (isprint(c))
        return S_ISPRINT;
    return (-1);
}

void    parse_err(int err_type, int col, int row, std::string line)
{
    for (int i = 0; i < col; ++i)
        std::cerr << " ";
    std::cerr << "v" << std::endl;
    std::cerr << line << std::endl;
    switch (err_type)
    {
        case E_UNEXP:
            std::cerr << "Error: Unexpected Token";
            break;
        case E_BRACKC_HIGH:
            std::cerr << "Error: Too Many Brackets";
            break;
        case E_BRACKC_LOW:
            std::cerr << "Error: Extra Bracket";
            break;
        case E_BRACKBAL:
            std::cerr << "Error: Brackets Imbalanced";
            break;
        case E_EQCOUNT:
            std::cerr << "Error: Too Many Assignments";
            break;
        case E_NOEQ:
            std::cerr << "Error: Only Key Provided";
            break;
    }
    std::cerr << " at R: " << row + 1 << " C: " << col + 1 << "\n" << std::endl;
}

int validate_context(std::string ctx, int row)
{
    std::map<int, std::vector<int> > state_transitions =
    {
        {S_OBRACK, {S_OBRACK, S_ALPHA}},
        {S_ALPHA, {S_ALPHA, S_DOT, S_CBRACK}},
        {S_DOT, {S_ALPHA}},
        {S_CBRACK, {S_CBRACK}}
    };
    int balance = 0;
    int cur_state = S_NONE;
    int next_state = S_NONE;
    std::map<int, std::vector<int> >::iterator map_idx;
    for (std::string::iterator it = ctx.begin(); it != ctx.end(); ++it)
    {
        cur_state = chr_to_ctx_state(*it);
        next_state = chr_to_ctx_state(*(it+1));
        map_idx = state_transitions.find(cur_state);
        if (it < ctx.end() - 1)
        {
            if (map_idx == state_transitions.end())
            {
                parse_err(E_UNEXP, it - ctx.begin() + 1, row, ctx);
                return (-1);
            }
            int good = 0;
            for ( auto a : state_transitions[cur_state])
            {
                if (a == next_state)
                    good = 1;
            }
            if (good != 1)
            {
                parse_err(E_UNEXP, it - ctx.begin() + 1, row, ctx);
                return (-1);
            }
        }
        switch (cur_state)
        {
            case S_OBRACK:
            {
                ++balance;
                if (balance > 2)
                {
                    parse_err(E_BRACKC_HIGH, it - ctx.begin(), row, ctx);
                    return (-1);
                }
                break;
            }
            case S_CBRACK:
            {
                --balance;
                if (balance < 0)
                {
                    parse_err(E_BRACKC_LOW, it - ctx.begin(), row, ctx);
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
        parse_err(E_BRACKBAL, ctx.size() - 1, row, ctx);
        return (-1);
    }
    return (0);
}

int validate_action(std::string act, int row)
{
    std::map<int, std::vector<int> > state_transitions =
    {
        {S_ISPRINT, {S_ISPRINT, S_EQ}},
        {S_EQ, {S_ISPRINT}}
    };
    int cur_state = S_NONE;
    bool eq_visited = false;
    int next_state = S_NONE;
    std::map<int, std::vector<int> >::iterator map_idx;

    for (std::string::iterator it = act.begin(); it != act.end(); ++it)
    {
        cur_state = chr_to_act_state(*it);
        next_state = chr_to_act_state(*(it+1));
        map_idx = state_transitions.find(cur_state);
        if (it < act.end() - 1)
        {
            if (map_idx == state_transitions.end())
            {
                parse_err(E_UNEXP, it - act.begin() + 1, row, act);
                return (-1);
            }
            int good = 0;
            for ( auto a : state_transitions[cur_state])
            {
                if (a == next_state)
                    good = 1;
            }
            if (good != 1)
            {
                parse_err(E_UNEXP, it - act.begin() + 1, row, act);
                return (-1);
            }
        }
        if (cur_state == S_EQ)
        {
            if (eq_visited)
            {
                parse_err(E_EQCOUNT, act.size() - 1, row, act);
                return (-1);
            }
            eq_visited = true;
        }
    }
    if (!eq_visited)
    {
        parse_err(E_NOEQ, act.size() - 1, row, act);
        return (-1);
    }
    return (0);
}

int perform_context(std::string ctx, int row, ctx_state* state)
{
    std::string processed;
    std::vector<std::string> map_ops;
    bool vec = false;
    Node* tmp = nullptr;

    if (ctx[1] == '[')
    {
        processed = ctx.substr(2, ctx.length() - 4);
        vec = true;
    }
    else
        processed = ctx.substr(1, ctx.length() - 2);
    map_ops = split(processed, std::string("."));
    state->current = state->root;
    for (size_t i = 0; i < map_ops.size(); ++i)
    {
        if (state->current->getType() == Node::MAP || state->current->getType() == Node::VECTOR)
        {
            if (i == map_ops.size() - 1)
            {
                tmp = &state->current->fetch(map_ops[i]);
                if (tmp == state->current)
                {
                    if (vec)
                    {
                        if (state->current->getType() == Node::VECTOR)
                            state->current->getVectorValue().push_back(new Node(Node::VECTOR));
                        else
                            state->current->getMapValue()[map_ops[i]] = new Node(Node::VECTOR);
                    }
                    else
                    {
                        if (state->current->getType() == Node::VECTOR)
                            state->current->getVectorValue().push_back(new Node(Node::MAP));
                        else
                            state->current->getMapValue()[map_ops[i]] = new Node(Node::MAP);
                    }
                    tmp = &state->current->fetch(map_ops[i]);
                }
                if (vec)
                {
                    if (tmp->getType() != Node::VECTOR)
                        throw "perform_context: Expected vector value";
                    tmp->getVectorValue().push_back(new Node(Node::MAP));
                    tmp = tmp->getVectorValue().back();
                }
                state->current = tmp;
            }
            else
            {
                tmp = &state->current->fetch(map_ops[i]);
                if (tmp == state->current)
                {
                    if (state->current->getType() == Node::VECTOR)
                        state->current->getVectorValue().push_back(new Node(Node::MAP));
                    else
                        state->current->getMapValue()[map_ops[i]] = new Node(Node::MAP);
                    tmp = &state->current->fetch(map_ops[i]);
                }
                state->current = tmp;
            }
        }
        else
            throw "perform_context: Unexpected lone string in tree";
    }
    return (0);
}

int perform_action(std::string act, int row, ctx_state* state)
{
    std::vector<std::string> splitted = split(act, "=");
    switch (state->current->getType())
    {
        case Node::VECTOR:
            throw "perform_action: Vector cannot act as endpoint";
            break;
        case Node::MAP:
            state->current->getMapValue()[splitted[0]] = new Node(splitted[1]);
            break;
        case Node::STRING:
            throw "perform_action: String cannot act as endpoint";
            break;
        default:
            throw "perform_action: Unknown action value";
    }
    return (0);
}

std::string remove_spaces(std::string i)
{
    std::string processed("");
    for ( std::string::iterator it = i.begin(); it != i.end(); ++it)
    {
        if (!(isspace(*it)))
            processed.push_back(*it);
    }
    return processed;
}

Node* construct_tree(std::string input)
{
    int errc = 0;
    int context_state = Node::MAP;
    std::vector<std::string>    splitted_input = split(input, std::string("\n"));
    Node* root = new Node(Node::MAP);
    ctx_state state = {context_state, root, root};

    for (int i = 0; i < splitted_input.size(); ++i)
    {
        std::string no_space = remove_spaces(splitted_input[i]);
        try
        {
            switch (match_input(splitted_input[i]))
            {
                case 1:
                    if (validate_context(no_space, i) == -1)
                        ++errc;
                    if (!errc)
                        perform_context(no_space, i, &state);
                    break;
                case 2:
                    if (validate_action(no_space, i) == -1)
                        ++errc;
                    if (!errc)
                        perform_action(no_space, i, &state);
                    break;
                default:
                    break;
            }
        }
        catch ( const std::exception &e )
        {
            std::cout << e.what() << std::endl;
        }
    }
    if (errc)
    {
        std::cerr << "Found a total of " << errc << " errors in the configuration!" << std::endl;
        exit(1);
    }
    return root;
}

int main(void)
{
    std::ifstream f("../config.tini");
    std::stringstream ss;
    std::map<std::string, std::string> gobal_map;
    
    ss << f.rdbuf();
    std::string input_string = ss.str();
    
    Node* root = construct_tree(input_string);
    std::map<std::string, Node*> root_value = root->getMapValue();
    root->print_contents(0, std::string("root"));
    delete root;
    return (0);
}
