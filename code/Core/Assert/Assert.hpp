#pragma once

// CPP_IGNORE

#include "Core/BasicTypes.hpp"

class AssertUtils
{
private:
	inline static const std::string smEmptyAssert = "?";

public:
	static void assertMsg(bool condition, const std::string& file, u32 line, const std::string& function,
					const std::string& message = smEmptyAssert)
	{
		if (!condition)
		{
			std::cout << "ASSERT > [" << function << ":"
					<< std::to_string(line) << "] > " << message << std::endl;
			throw;
			//abort();
		}
	}
};

#define ASSERT_MSG(condition, message) AssertUtils::assertMsg((condition), __FILE__, __LINE__, __FUNCTION__, std::string("[ " #condition " ] ") + message);
