#include "Core/Assert/Assert.hpp"
#include "Core/Log/Log.hpp"
#include <csignal>

void AssertUtils::checkMsg(bool condition, const std::string& conditionString, const std::string& file, u32 line, const std::string& function, const std::string& message /*= smEmptyAssert*/)
{
	if (!condition)
	{
		std::string assertMsg = "[" + file + ":" + function + ":" + std::to_string(line) + "]";
		assertMsg += "[" + conditionString + "] ";
		assertMsg += message;
        Log::customEcho("ASSERT", assertMsg);
		sendAssertSignal();
	}
}

void AssertUtils::sendAssertSignal()
{
    std::raise(SIGTRAP);
}
