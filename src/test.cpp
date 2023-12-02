#include <cstdlib>
#include <iostream>
#include <vector>
#include "RequestLine.hpp"
#include "Reader.hpp"



using std::vector;
using std::string;

#define DELIM "\t "

#define assert_throw(expression, exception_type) \
    try { expression; assert(false); } \
    catch (exception_type) { assert(true); }

int main()
{
    try {
        ReaderTests::line_one();
        ReaderTests::line_noline();
        ReaderTests::line_empty();
        ReaderTests::line_basic();

        HTTPVersionTests::all();

        static const char content[] = "GET     \t\t\t   /\t\t      HTTP/1.1  \t\t\t\t    \r\n";
        vector<uint8_t> buffer(content, content + sizeof(content) / sizeof(*content));
        Reader reader(buffer);

        RequestLine line(reader.line());

        std::cout << line.method() << ' ' << line.uri() << ' ' << line.http_version() << '\n';

        std::cout << "Tests ok!\n";
    }
    catch (const char* failed_test)
    {
        std::cerr << "Test failed: " << failed_test << '\n';
    }
    catch (const std::string& failed_test) {
        std::cerr << "Test failed: " << failed_test << '\n';
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return EXIT_SUCCESS;
}
