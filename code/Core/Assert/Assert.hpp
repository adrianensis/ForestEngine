#pragma once

#include "Core/BasicTypes.hpp"

#ifdef CPP_INCLUDE
#include "Core/Assert/Assert.hpp"
#endif

class AssertUtils
{
private:
	inline static const std::string smEmptyAssert = "?";

public:
	CPP static void assertMsg(bool condition, const std::string& file, u32 line, const std::string& function, const std::string& message = smEmptyAssert)
	{
		if (!condition)
		{
			// TODO: use Log!!
			std::cout << "ASSERT > [" << function << ":"
					<< std::to_string(line) << "] > " << message << std::endl;
			throw;
		}
	}
};

#define ASSERT_MSG(condition, message) AssertUtils::assertMsg((condition), __FILE__, __LINE__, __FUNCTION__, std::string("[ " #condition " ] ") + message);
