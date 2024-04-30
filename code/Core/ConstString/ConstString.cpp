#pragma once

#include "Core/ConstString/ConstString.hpp"

ConstString::ConstString(const char* str)
{
    if(std::strlen(str) == 0)
    {
        return;
    }

    mHash = Hash::hashString(str);
    if(!ConstStringsManager::containsString(mHash))
    {
        ConstStringsManager::registerString(str);
    }
}
ConstString::ConstString(const std::string_view& str) : ConstString(str.data())
{
}

const std::string& ConstString::get() const
{
    if(isValid())
    {
        return ConstStringsManager::getString(mHash);
    }

    return ConstStringsManager::mNullString;
}

bool ConstString::isValid() const
{
    return mHash > 0;
}
