#include <iterator>
#include <stdexcept>
#include <utility>
#include <optional>
#include <string>
#include "Buffer.hpp"
#include "Reader.hpp"

using std::optional;
using std::string;

const char* Reader::LineLimitError::what() const noexcept
{
    return "line length exceeds limit argument";
}

Reader::Reader(Buffer&& buffer) : _buffer(std::move(buffer)), _head(_buffer.begin()) {}

void Reader::trim_empty_lines()
{
    while (_head != _buffer.end())
    {
        if (*_head == '\r')
        {
            if (std::next(_head) == _buffer.end() || *std::next(_head) != '\n')
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

optional<string> Reader::line(size_t limit)
{
    Buffer::iterator start = _head;
    Buffer::iterator pos = _head;

    while (pos != _buffer.end())
    {
        if (*pos == '\r')
        {
            std::advance(pos, 1);
            if (pos == _buffer.end())
            {
                return std::nullopt;
            }
            if (*pos == '\n')
            {
                _head = std::next(pos);
                return string(start, std::prev(pos));
            }
            if (limit-- == 0)
                throw std::runtime_error("line too long");
            *std::prev(pos) = ' ';
            continue;
        }
        if (*pos == '\n')
        {
            _head = std::next(pos);
            return string(start, pos);
        }
        if (limit-- == 0)
            throw std::runtime_error("line too long");
        std::advance(pos, 1);
    }
    return std::nullopt;
}

Buffer& Reader::buffer()
{
    return _buffer;
}

const Buffer& Reader::buffer() const
{
    return _buffer;
}

#ifdef TEST

#include <cstring>
#include "testutils.hpp"

Buffer ReaderTest::buffer(const std::string& content)
{
    return Buffer(content.begin(), content.end());
}

void ReaderTest::line_empty_test()
{
    BEGIN

    Reader reader(buffer("\r\n"));

    EXPECT(reader.line().value() == "");

    END
}

void ReaderTest::line_noline_test()
{
    BEGIN

    Reader reader(buffer("\naaaa"));

    EXPECT(reader.line());
    EXPECT(!reader.line());

    END
}

void ReaderTest::line_basic_test()
{
    BEGIN

    Reader reader(buffer("aa\nbb\r\n"));

    EXPECT(reader.line() == "aa");
    EXPECT(reader.line() == "bb");

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

void ReaderTest::trim_empty_lines_test()
{
    BEGIN

    const char* content = "\r\nline1\r\n"
                          "\r\nline2\r\n";
    Reader      reader(buffer(content));

    EXPECT(reader.line() == "");
    EXPECT(reader.line() == "line1");

    reader.trim_empty_lines();
    EXPECT(reader.line() == "line2");

    END
}

void ReaderTest::line_limit_test()
{
    BEGIN

    const char* content = "ab\r\n"
                          "ab\n"
                          "ab\r\n"
                          "ab\n";
    Reader      reader(buffer(content));

    EXPECT(reader.line(2) == "ab");
    EXPECT(reader.line(2) == "ab");

    try
    {
        (void)reader.line(1);
        EXPECT(false);
    }
    catch (const std::runtime_error&)
    {
    }

    try
    {
        (void)reader.line(1);
        EXPECT(false);
    }
    catch (const std::runtime_error&)
    {
    }

    END
}

#endif
