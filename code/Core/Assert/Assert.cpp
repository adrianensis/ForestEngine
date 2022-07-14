#include "Core/Assert/Assert.hpp"


void AssertUtils::assertMsg(bool condition, CR(std::string) file, u32 line, CR(std::string) function, CR(std::string) message /*= smEmptyAssert*/)
{
	if (!condition)
	{
		std::cout << "ASSERT > [" << function << ":"
				<< std::to_string(line) << "] > " << message << std::endl;
		throw;
	}
}
