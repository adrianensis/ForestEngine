#pragma once

#include "Core/ConstString/ConstString.hpp"

ConstString::ConstString(const std::string_view& str)
{
    if(str.empty())
    {
        return;
    }

    mHash = Hash::hashString(str);
    if(!ConstStringsManager::containsString(mHash))
    {
        ConstStringsManager::registerString(str);
    }
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
