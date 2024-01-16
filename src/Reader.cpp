#include <iterator>
#include "Reader.hpp"

using std::string;
using std::vector;

Reader::Reader(const vector<char>& buffer) : _buffer(buffer), _head(_buffer.begin()) {}

Reader::Reader(vector<char>&& buffer) : _buffer(std::move(buffer)), _head(_buffer.begin()) {}

void Reader::trim_empty_lines()
{
    while (_head != _buffer.end())
    {
        if (*_head == '\r')
        {
            if (std::next(_head) == _buffer.end() || *std::next(_head) == '\n')
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

char* Reader::data()
{
    return _buffer.data();
}

std::string Reader::line()
{
    vector<char>::iterator start = _head;
    vector<char>::iterator pos = _head;

    while (pos != _buffer.end())
    {
        if (*pos == '\r')
        {
            std::advance(pos, 1);
            if (pos == _buffer.end())
            {
                throw ReaderException(ReaderException::NoLine);
            }
            if (*pos == '\n')
            {
                _head = std::next(pos);
                return string(start, std::prev(pos));
            }
            *std::prev(pos) = ' ';
            continue;
        }
        if (*pos == '\n')
        {
            _head = std::next(pos);
            return string(start, pos);
        }
        std::advance(pos, 1);
    }
    throw ReaderException(ReaderException::NoLine);
}

ReaderException::ReaderException(Type type) noexcept : _type(type) {}

ReaderException::Type ReaderException::type() const noexcept
{
    return _type;
}

const char* ReaderException::what() const noexcept
{
    return "No line in buffer\n";
}

#ifdef TEST

#include <cstring>
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
