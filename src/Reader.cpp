#include <cstring>
#include <sys/types.h>
#include "Reader.hpp"

using std::distance;
using std::string;
using std::vector;

Reader::Reader(const vector<char>& buffer, size_t position)
    : _buffer(buffer), _head(_buffer.begin())
{
}

Reader::Reader(vector<char>&& buffer, size_t position)
    : _buffer(std::move(buffer)), _head(_buffer.begin())
{
}

Reader& Reader::trim(const std::string& charset) throw()
{
    while (_head != _buffer.end() && charset.find(*_head) != string::npos)
    {
        _head++;
    }
    return *this;
}

void Reader::trim_empty_lines()
{
    while (_head != _buffer.end())
    {
        if (*_head == '\r')
        {
            if (next() == _buffer.end() || *next() == '\n')
                break;
            std::advance(_head, 2);
        }
        else if (*_head == '\n')
        {
            std::advance(_head, 1);
        }
        else
            break;
    }
}

vector<char>::const_iterator Reader::next()
{
    return _head + 1;
}

void Reader::consume(size_t amount)
{
    if (std::distance(_head, _buffer.end()) > (ssize_t)amount)
        throw "Out of bounds";
    _head += amount;
}

char* Reader::data()
{
    return _buffer.data();
}

// Return a line from the internal buffer
// Throws no line, if the buffer does not contain a line
std::string Reader::line()
{
    vector<char>::iterator start = _head;
    vector<char>::iterator pos = _head;

    while (pos != _buffer.end())
    {
        if (*pos == '\r')
        {
            pos++;
            if (pos == _buffer.end())
            {
                throw ReaderException(ReaderException::NoLine);
            }
            if (*pos == '\n')
            {
                _head = pos + 1;
                return string(start, pos - 1);
            }
            pos[-1] = ' ';
            continue;
        }
        if (*pos == '\n')
        {
            _head = pos + 1;
            return string(start, pos);
        }
        pos++;
    }
    throw ReaderException(ReaderException::NoLine);
}

ReaderException::ReaderException(Type type) throw() : _type(type) {}

ReaderException::Type ReaderException::type() const throw()
{
    return _type;
}

const char* ReaderException::what() const throw()
{
    return "No line in buffer\n";
}

#ifdef TEST

#include "testutils.hpp"

vector<char> ReaderTest::buffer(const char* content)
{
    return vector<char>(content, content + std::strlen(content));
}

void ReaderTest::line_empty_test()
{
    BEGIN

    vector<char> buffer = ReaderTest::buffer("\n");
    Reader       reader(buffer);

    EXPECT(reader.line() == "");

    END
}

void ReaderTest::line_noline_test()
{
    BEGIN

    vector<char> buffer = ReaderTest::buffer("aaaa");
    Reader       reader(buffer);

    try
    {
        string line = reader.line();
    }
    catch (const ReaderException& error)
    {
        if (error.type() != ReaderException::NoLine)
            throw error;
        return;
    }
    EXPECT(false);

    END
}

void ReaderTest::line_one_test()
{
    BEGIN

    vector<char> buffer = ReaderTest::buffer("a\n");
    Reader       reader(buffer);

    EXPECT(reader.line() == "a");

    END
}

void ReaderTest::line_basic_test()
{
    BEGIN

    vector<char> buffer = ReaderTest::buffer("aa\nbb\r\n");
    Reader       reader(buffer);

    string       line1 = reader.line();
    string       line2 = reader.line();

    EXPECT(line1 == "aa");
    EXPECT(line2 == "bb");

    END
}

void ReaderTest::line_strip_bare_cr_test()
{
    BEGIN

    const char* content = "\rbare cr1 \r\n"           // One beginning
                          " bare\rcr2 \r\n"           // One middle
                          " bare cr3\r\r\n"           // One end
                          "\rbare\rcr4\r\r\n"         // One each
                          "\r\rmulti  cr1  \r\n"      // Multiple beginning
                          "  multi\r\rcr2  \r\n"      // Multiple middle
                          "  multi  cr3\r\r\r\n"      // Multiple end
                          "\r\rmulti\r\rcr4\r\r\r\n"; // Multiple each

    Reader reader(buffer(content));

    EXPECT(reader.line() == " bare cr1 ");
    EXPECT(reader.line() == " bare cr2 ");
    EXPECT(reader.line() == " bare cr3 ");
    EXPECT(reader.line() == " bare cr4 ");
    EXPECT(reader.line() == "  multi  cr1  ");
    EXPECT(reader.line() == "  multi  cr2  ");
    EXPECT(reader.line() == "  multi  cr3  ");
    EXPECT(reader.line() == "  multi  cr4  ");

    END
}

#endif
