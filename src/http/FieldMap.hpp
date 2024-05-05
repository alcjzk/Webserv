#pragma once

#include <utility>
#include <unordered_map>
#include "FieldValue.hpp"
#include "FieldName.hpp"

class FieldMap
{
    public:
        using Container = std::unordered_map<FieldName, FieldValue>;
        using Entry = std::pair<FieldName, FieldValue>;
        using iterator = Container::iterator;
        using const_iterator = Container::const_iterator;

        const FieldValue* get(const FieldName& name) const;
        bool              insert(Entry&& entry);
        bool              insert(const Entry& entry);
        bool              erase(const FieldName& name);

        iterator       begin() noexcept;
        const_iterator begin() const noexcept;

        iterator       end() noexcept;
        const_iterator end() const noexcept;

    private:
        Container _container;
};
