#pragma once

#include "Core/Memory.hpp"

class ObjectBase;

using ClassRegisterCallback = std::function<ObjectBase*()>;

class ClassRegister;
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
    inline static std::map<std::string_view, ClassRegister> mClassMap;
};

class ClassRegister
{
public:
    ClassRegister(const std::string_view& className, const ClassRegisterCallback& callback)
    {
        mClassName = className;
        mCallback = callback;
        MAP_INSERT(ClassManager::mClassMap, className, *this);
    }

    std::string_view mClassName;
    ClassRegisterCallback mCallback;
};