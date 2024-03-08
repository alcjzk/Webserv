#include <string>
#include <unordered_map>
#include <regex>

class TemplateEngine
{
    public:
        TemplateEngine(const std::string& template_str);
        TemplateEngine() = delete;
        TemplateEngine(const TemplateEngine& other) = default;
        TemplateEngine(TemplateEngine&& other) noexcept = default;
        TemplateEngine& operator=(const TemplateEngine& other) = default;
        TemplateEngine& operator=(TemplateEngine&& other) noexcept = default;
        ~TemplateEngine() = default;

        void        set_value(const std::string& key, const std::string& value);
        std::string render();

    private:
        std::string                                  _template_str;
        std::unordered_map<std::string, std::string> _data;
};
