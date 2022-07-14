#ifndef ASSERT_HPP
#define ASSERT_HPP

#include "Core/BasicTypes.hpp"

class AssertUtils
{
private:
	inline static const std::string smEmptyAssert = "?";

public:
    static void assertMsg(bool condition, CR(std::string) file, u32 line, CR(std::string) function, CR(std::string) message = smEmptyAssert);
};

#define ASSERT_MSG(condition, message) AssertUtils::assertMsg((condition), __FILE__, __LINE__, __FUNCTION__, std::string("[ " #condition " ] ") + message);

#endif