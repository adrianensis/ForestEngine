#include "Core/ClassManager.hpp"

ClassRegister::ClassRegister(const std::string_view& className, const ClassRegisterCallback& callback)
{
    mClassName = className;
    mCallback = callback;
    MAP_INSERT(ClassManager::mClassMap, className, *this);
}

ObjectBase* ClassManager::instance(const std::string_view& className)
{
    return mClassMap.at(className).mCallback();
}