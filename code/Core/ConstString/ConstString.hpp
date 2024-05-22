#pragma once

#include "Core/StdCore.hpp"
#include "Core/Assert/Assert.hpp"
#include <unordered_map>

class ConstString
{
public:
    ConstString() = default;
    ConstString(const std::string_view& str);
    ConstString(const char* str);
    const std::string& get() const;
    bool isValid() const;
    HashValue getHash() const { return mHash; }
    const std::string& operator*() const { CHECK_MSG(this->isValid(), "Invalid string!"); return get(); }
    const std::string* operator->() const { return &(operator*()); }
    bool operator==(const ConstString& other) const { return this->mHash == other.mHash; }
    ConstString& operator=(const ConstString& other)
    {
        if (this != &other)
        {
            this->mHash = other.mHash;
            #ifdef ENGINE_DEBUG
            this->mString = other.mString;
            #endif
        }
        return *this;
    }
private:
    HashValue mHash = 0;
    #ifdef ENGINE_DEBUG
    std::string mString;
    #endif
};

template<>
struct std::hash<ConstString> 
{
    size_t operator()(const ConstString& str) const 
    {
        return str.getHash();
    }
};

class ConstStringsManager
{
public:
    static bool containsString(HashValue hashValue)
    {
        return mHashToString.contains(hashValue);
    }
    static const std::string& getString(HashValue hashValue)
    {
        return mHashToString.at(hashValue);
    }
	static HashValue registerString(const std::string_view& str)
    {
        if(str.empty())
        {
            return 0;
        }
        
        HashValue stringHash = Hash::hashString(str);
        mHashToString.emplace(stringHash, str);
        return stringHash;
    }

private:
	inline static std::unordered_map<HashValue, std::string> mHashToString;
public:
	inline static const std::string mNullString;
};