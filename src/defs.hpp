#pragma once

// TODO: Move elsewhere

#define HTTP_FIELD_DELIMETER " \t"

#ifdef TESTS
# include <sstream>
# define TESTFAIL static_cast<std::ostringstream>((std::ostringstream() << __FILE__ << "::" << __FUNCTION__ << "()")).str()
#endif
