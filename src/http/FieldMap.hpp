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

        const FieldValue* get(const FieldName& name) const;
        bool              insert(Entry&& entry);
        bool              insert(const Entry& entry);
        bool              erase(const FieldName& name);

    private:
        Container _container;
};
