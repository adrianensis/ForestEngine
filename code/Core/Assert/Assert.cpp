#include "Core/Assert/Assert.hpp"


void AssertUtils::assertMsg(bool condition, const String& file, u32 line, const String& function, const String& message /*= smEmptyAssert*/)
{
	if (!condition)
	{
		std::cout << "ASSERT > [" << function << ":"
				<< std::to_string(line) << "] > " << message << std::endl;
		throw;
	}
}
