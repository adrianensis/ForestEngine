#include "Core/Assert/Assert.hpp"
#include "Core/Log/Log.hpp"
#include <csignal>

void AssertUtils::checkMsg(bool condition, const std::string& conditionString, const std::string& file, unsigned int line, const std::string& function, const std::string& message)
{
	if (!condition)
	{
		std::string assertMsg = "[" + file + ":" + function + ":" + std::to_string(line) + "]";
		assertMsg += "[" + conditionString + "] ";
		assertMsg += message;
        Log::log(Log::Prefixes::smAssert, assertMsg, true);
		sendAssertSignal();
	}
}

void AssertUtils::sendAssertSignal()
{
    std::raise(SIGTRAP);
}
