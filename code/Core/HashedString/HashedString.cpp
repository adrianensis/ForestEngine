#include "Core/HashedString/HashedString.hpp"

HashedString::HashedString(const char* str)
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
    mString = &get();
    #endif
}
HashedString::HashedString(const std::string_view& str) : HashedString(str.data())
{
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
