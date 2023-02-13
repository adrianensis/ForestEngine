#pragma once

#include "Core/Memory.hpp"

class ObjectBase;

using ClassRegisterCallback = std::function<ObjectBase*()>;

#define REGISTER_CLASS_BY_NAME(...) \
    ClassRegister classRegister_##__VA_ARGS__ {__VA_ARGS__::getClassNameStatic(), [](){ \
        if constexpr (std::is_abstract_v<__VA_ARGS__>) { return nullptr; } \
        else { return Memory::newObject<__VA_ARGS__>(); } \
    }};

class ClassRegister
{
public:
    ClassRegister(const std::string_view& className, const ClassRegisterCallback& callback);

    std::string_view mClassName;
    ClassRegisterCallback mCallback;
};

class ClassManager
{
friend ClassRegister;

public:
    template<typename T>
    static T* instance(const std::string_view& className)
    {
        return static_cast<T*>(instance(className));
    }

    static ObjectBase* instance(const std::string_view& className);

private:
    inline static std::unordered_map<std::string_view, ClassRegister> mClassMap;
};