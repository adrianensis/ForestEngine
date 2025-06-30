#include "Core/HashedString/HashedString.hpp"

// NS_BEGIN(Core)
HashedString::HashedString(const char* str)
{
    set(str);
}
HashedString::HashedString(const std::string& str) : HashedString(str.data())
{
}
HashedString::HashedString(const std::string_view& str) : HashedString(str.data())
{
}

void HashedString::set(const char* str)
{
    if(std::strlen(str) == 0)
    {
        return;
    }

    mHash = Hash::hashString(str);
    if(!HashedStringsManager::containsString(mHash))
    {
        HashedStringsManager::registerString(str);
    }

    #ifdef ENGINE_BUILD_DEBUG
    mDebugString = &get();
    #endif
}

const std::string& HashedString::get() const
{
    if(isValid())
    {
        return HashedStringsManager::getString(mHash);
    }

    return HashedStringsManager::mNullString;
}

bool HashedString::isValid() const
{
    return mHash > 0;
}

// NS_END