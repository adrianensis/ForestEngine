#pragma once

#include "Core/StdCore.hpp"
#include "Core/Log/Log.hpp"
#include "Core/Signal/Signal.hpp"
#include "Core/StdMacros.hpp"

NS_BEGIN(Core)

class AssertUtils
{
public:
	template <typename... T>
    static void checkMsg(bool condition, const std::string& conditionString, const std::string& file, Core::u32 line, const std::string& function, const std::string& fmt, T&&... args)
    {
        if (!condition)
        {
            std::string composedFmt = "[" + file + ":" + function + ":" + std::to_string(line) + "]";
            composedFmt += "[" + conditionString + "] ";
            composedFmt += fmt;
            Log::log(Log::Prefixes::smAssert, true, composedFmt, args...);
            SignalUtils::breakpointTrap();
        }
    }
};

#define CHECK_MSG(condition, ...) Core::AssertUtils::checkMsg((condition), #condition, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);

NS_END