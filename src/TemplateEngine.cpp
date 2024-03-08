#include "TemplateEngine.hpp"

TemplateEngine::TemplateEngine(const std::string& _template_str) : _template_str(_template_str) {}

void TemplateEngine::set_value(const std::string& key, const std::string& value)
{
    _data[key] = value;
}

std::string TemplateEngine::render()
{
    std::string          rendered = _template_str;
    std::regex           pattern(R"(\{\{(\w+)\}\})");

    std::sregex_iterator begin(rendered.begin(), rendered.end(), pattern);
    std::sregex_iterator end;

    while (begin != end)
    {
        std::smatch match = *begin;
        std::string key = match[1].str();

        if (_data.find(key) != _data.end())
            rendered.replace(match.position(), match.length(), _data[key]);
        ++begin;
    }

    return rendered;
}