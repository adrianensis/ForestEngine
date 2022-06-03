#ifndef ASSERT_HPP
#define ASSERT_HPP

#include "Core/BasicTypes.hpp"

class AssertUtils
{
private:
	inline static const String smEmptyAssert = "?";

public:
    static void assertMsg(bool condition, const String& file, u32 line, const String& function, const String& message = smEmptyAssert);
};

#define ASSERT_MSG(condition, message) AssertUtils::assertMsg((condition), __FILE__, __LINE__, __FUNCTION__, String("[ " #condition " ] ") + message);

#endif