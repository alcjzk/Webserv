#include <regex>
#include "TemplateEngine.hpp"

TemplateEngine::TemplateEngine(const std::string& _template_str) : _template_str(_template_str) {}

void TemplateEngine::set_value(const std::string& key, const std::string& value)
{
    _data[key] = value;
}

std::string TemplateEngine::render()
{
    std::string rendered = _template_str;
    std::regex  pattern(R"(\{\{(\w*)\}\})");
    std::smatch match;

    while (std::regex_search(rendered, match, pattern))
    {
        std::string placeholder = match[1];
        std::string regex_pattern = R"(\{\{)" + placeholder + R"(\}\})";
        if (_data.find(placeholder) != _data.end())
            rendered = std::regex_replace(rendered, std::regex(regex_pattern), _data[placeholder]);
        else
            rendered = std::regex_replace(rendered, std::regex(regex_pattern), "UNDEFINED");
    }
    return rendered;
}

#ifdef TEST

#include "testutils.hpp"

void TemplateEngineTest::single_replace_test()
{
    BEGIN

    TemplateEngine engine("Replace {{me}} with something");
    engine.set_value("me", "asparagus");
    EXPECT(engine.render() == "Replace asparagus with something");

    END
}

void TemplateEngineTest::multiple_same_replace_test()
{
    BEGIN

    TemplateEngine engine("{{me}}{{me}}{{me}}");
    engine.set_value("me", "you");
    EXPECT(engine.render() == "youyouyou");

    END
}

void TemplateEngineTest::multiple_different_replace_test()
{
    BEGIN

    TemplateEngine engine("{{one}}{{two}}{{three}}");
    engine.set_value("one", "three");
    engine.set_value("two", "one");
    engine.set_value("three", "two");
    EXPECT(engine.render() == "threeonetwo");

    END
}

void TemplateEngineTest::empty_replace_test()
{
    BEGIN

    TemplateEngine engine("{{}}");
    engine.set_value("", "void");
    EXPECT(engine.render() == "void");

    END
}

void TemplateEngineTest::nested_empty_test()
{
    BEGIN

    TemplateEngine engine("{{{{}}}}");
    EXPECT(engine.render() == "UNDEFINED");

    END
}

#endif
