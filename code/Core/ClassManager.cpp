#include "Core/ClassManager.hpp"

ObjectBase* ClassManager::instance(const std::string_view& className)
{
    return mClassMap.at(className).mCallback();
}