#pragma once
#include <unordered_map>
#include <typeindex>
namespace Core {
class Container {
public:
    template<typename T>
    static void Set(T* instance) { registry()[std::type_index(typeid(T))] = instance; }
    template<typename T>
    static T* Get() { auto it = registry().find(std::type_index(typeid(T))); return it == registry().end() ? nullptr : static_cast<T*>(it->second); }
private:
    static std::unordered_map<std::type_index, void*>& registry() { static std::unordered_map<std::type_index, void*> r; return r; }
};
}

