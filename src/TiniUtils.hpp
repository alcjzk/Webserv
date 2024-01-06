#include <iostream>
#include <string>
#include <vector>

#ifndef TINI_UTILS_HPP
#define TINI_UTILS_HPP

std::vector<std::string> split(std::string s, std::string d);
int match_input(std::string input);
int chr_to_ctx_state(char c);
int chr_to_act_state(char c);
std::vector<std::string> removeSpaces(std::vector<std::string> split_input);
void printErr(int err_type, int col, int row, std::string line);

#endif
