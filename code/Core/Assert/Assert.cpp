#include "Core/Assert/Assert.hpp"


void AssertUtils::assertMsg(bool condition, const std::string& file, u32 line, const std::string& function, const std::string& message /*= smEmptyAssert*/)
{
	if (!condition)
	{
		std::cout << "ASSERT > [" << function << ":"
				<< std::to_string(line) << "] > " << message << std::endl;
		throw;
	}
}
