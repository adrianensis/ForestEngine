#pragma once

#include "Core/StdCore.hpp"
#include "Core/Assert/Assert.hpp"
#include <unordered_map>

NS_BEGIN(Core)
class HashedString
{
public:
    HashedString() = default;
    HashedString(const std::string_view& str);
    HashedString(const std::string& str);
    HashedString(const char* str);
    const std::string& get() const;
    bool isValid() const;
    HashValue getHash() const { return mHash; }
    const std::string& operator*() const { CHECK_MSG(this->isValid(), "Invalid string!"); return get(); }
    const std::string* operator->() const { return &(operator*()); }
    bool operator==(const HashedString& other) const { return this->mHash == other.mHash; }
    HashedString& operator=(const HashedString& other)
    {
        if (this != &other)
        {
            this->mHash = other.mHash;
            #ifdef ENGINE_BUILD_DEBUG
            this->mDebugString = other.mDebugString;
            #endif
        }
        return *this;
    }

private:
    void set(const char* str);

private:
    HashValue mHash = 0;
    #ifdef ENGINE_BUILD_DEBUG
    const std::string* mDebugString = nullptr;
    #endif

public:
    #ifdef ENGINE_BUILD_DEBUG
    const std::string& getDebugString() const { return *mDebugString;};
    #endif
};

class HashedStringsManager
{
public:
    static void terminate()
    {
        mHashToString.clear();
    } 

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
NS_END

template<>
struct std::hash<Core::HashedString> 
{
    size_t operator()(const Core::HashedString& str) const 
    {
        return str.getHash();
    }
};