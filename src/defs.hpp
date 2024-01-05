#pragma once

// TODO: Move elsewhere

#define HTTP_FIELD_DELIMETER " \t" // TODO: Replace with below
#define HTTP_LWS " \t"

#ifdef TESTS
# include <sstream>
# define TESTFAIL static_cast<std::ostringstream>((std::ostringstream() << __FILE__ << "::" << __FUNCTION__ << "()")).str()
#endif

#define HT '\t'
#define SP ' '
#define LF '\n'
#define CR '\r'
#define DEL 127

