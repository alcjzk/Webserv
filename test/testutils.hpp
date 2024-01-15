#pragma once

#include <iostream>
#include <stdlib.h>
#include <exception>

#define FAIL                                                                                       \
    std::cerr << "\nExpectation failed: " << __FUNCTION__ << " in " << __FILE__ << " line "        \
              << __LINE__
#define PASS      std::cout << "."

#define EXPECT(x) if (x) PASS; else {FAIL; exit(1); };
#define BEGIN                                                                                      \
    try                                                                                            \
    {
#define END                                                                                        \
    }                                                                                              \
    catch (const std::exception& e)                                                                \
    {                                                                                              \
        std::cerr << "\nUnexpected exception: " << e.what() << " in " << __FILE__ << " line "      \
                  << __LINE__;                                                                     \
    }                                                                                              \
    catch (...)                                                                                    \
    {                                                                                              \
        std::cerr << "\nUnexpected non-standard exception in " << __FILE__ << " line "             \
                  << __LINE__;                                                                     \
    }

#define SKIP std::cout << "S"; return;
