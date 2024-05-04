#include "FieldMap.hpp"
#include "FieldValue.hpp"

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

bool FieldMap::erase(const FieldName& name)
{
    return _container.erase(name) == 1;
}
