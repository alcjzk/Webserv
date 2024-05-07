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

        /// Inserts an entry into the map. Returns false on failure.
        [[nodiscard]] bool insert(Entry&& entry);
        /// Inserts an entry into the map. Returns false on failure.
        [[nodiscard]] bool insert(const Entry& entry);

        void insert_or_assign(Entry&& entry);
        void insert_or_assign(const Entry& entry);

        bool erase(const FieldName& name);

        iterator       begin() noexcept;
        const_iterator begin() const noexcept;

        iterator       end() noexcept;
        const_iterator end() const noexcept;

    private:
        Container _container;
};
