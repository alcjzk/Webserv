#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace tiniutils
{
    std::vector<std::string> split(std::string raw_string, std::string delimiter, char escape = '\0');
    int                      match_input(std::string input);
    int                      chr_to_ctx_state(char c, char p);
    int                      chr_to_act_state(char c);
    std::vector<std::string> removeSpaces(std::vector<std::string> split_input);
    void                     printErr(int err_type, int col, int row, std::string line);
}

namespace tiniutilsTest
{
    void              split_basic_test();
    void              split_weird_delim_test();
    void              split_multichar_delim_test();
    void              split_none_test();
    void              split_basic_escape_test();
}
