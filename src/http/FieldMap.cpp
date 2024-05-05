#include "FieldMap.hpp"
#include "FieldValue.hpp"

using iterator = FieldMap::iterator;
using const_iterator = FieldMap::const_iterator;

const FieldValue* FieldMap::get(const FieldName& name) const
{
    if (auto it = _container.find(name); it != _container.end())
        return &(it->second);
    return nullptr;
}

bool FieldMap::insert(Entry&& entry)
{
    return _container.insert(std::move(entry)).second;
}

bool FieldMap::insert(const Entry& entry)
{
    return _container.insert(entry).second;
}

void FieldMap::insert_or_assign(Entry&& entry)
{
    (void)_container.insert_or_assign(entry.first, std::move(entry.second));
}

void FieldMap::insert_or_assign(const Entry& entry)
{
    (void)_container.insert_or_assign(entry.first, entry.second);
}

bool FieldMap::erase(const FieldName& name)
{
    return _container.erase(name) == 1;
}

iterator FieldMap::begin() noexcept
{
    return _container.begin();
}

const_iterator FieldMap::begin() const noexcept
{
    return _container.begin();
}

iterator FieldMap::end() noexcept
{
    return _container.end();
}

const_iterator FieldMap::end() const noexcept
{
    return _container.end();
}
