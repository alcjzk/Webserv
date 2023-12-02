#include "Reader.hpp"

using std::vector;
using std::string;
using std::distance;

Reader::Reader(const vector<char>& buffer, size_t position) : _buffer(buffer), _head(_buffer.begin())
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
                break ;
            std::advance(_head, 2);
        }
        else if (*_head == '\n')
        {
            std::advance(_head, 1);
        }
        else break ;
    }
}

vector<char>::const_iterator Reader::next()
{
    return _head + 1;
}

string  Reader::token(const string& delimeters, size_t limit)
{
    this->trim(delimeters);
    vector<char>::const_iterator token_end = _head;
    while (token_end != _buffer.end() && delimeters.find(*token_end) == string::npos)
    {
        if (limit-- == 0)
            throw "No token";
        token_end++;
    }
    if (token_end == _head)
        throw "No token";
    vector<char>::const_iterator token_start = _head;
    _head = token_end;
    if (_head != _buffer.end())
        _head++;
    return string(token_start, token_end);
}

void Reader::consume(size_t amount)
{
    if (std::distance(_head, _buffer.end()) > (ssize_t)amount)
        throw "Out of bounds";
    _head += amount;
}

// Return a line from the internal buffer
// Throws no line, if the buffer does not contain a line
std::string Reader::line()
{
    vector<char>::const_iterator start = _head;
    vector<char>::const_iterator end = _head;

    while (end != _buffer.end())
    {
        if (*end == '\n')
        {
            if (start == end)
            {
                _head = end + 1;
                return string();
            }
            _head = end + 1;
            if (*(end - 1) == '\r')
                end -= 1;
            return string(start, end);
        }
        end++;
    }
    throw ReaderException(ReaderException::NoLine);
}

ReaderException::ReaderException(Type type) throw() : _type(type)
{

}

ReaderException::Type ReaderException::type() const throw()
{
    return _type;
}

const char* ReaderException::what() const throw()
{
    return "No line in buffer\n";
}

#ifdef TESTS

vector<char> ReaderTests::buffer(const char* content)
{
    return vector<char>(content, content + std::strlen(content));
}

void ReaderTests::line_empty()
{
    vector<char> buffer = ReaderTests::buffer("\n");
    Reader reader(buffer);

    if (reader.line() != "")
        throw __FUNCTION__;
}

void ReaderTests::line_noline()
{
    vector<char> buffer = ReaderTests::buffer("aaaa");
    Reader reader(buffer);

    try {
        string line = reader.line();
        throw 0;
    }
    catch (int) {
        throw __FUNCTION__;
    }
    catch (...) {  }
}

void ReaderTests::line_one()
{
    vector<char> buffer = ReaderTests::buffer("a\n");
    Reader reader(buffer);

    if (reader.line() != "a")
        throw __FUNCTION__;
}

void ReaderTests::line_basic()
{
    vector<char> buffer = ReaderTests::buffer("aa\nbb\r\n");
    Reader reader(buffer);

    string line1 = reader.line();
    string line2 = reader.line();

    if (line1 != "aa" || line2 != "bb")
        throw __FUNCTION__;
}

#endif
