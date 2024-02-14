#pragma once

#include <string>

class AssertUtils
{
public:
    static void checkMsg(bool condition, const std::string& conditionString, const std::string& file, unsigned int line, const std::string& function, const std::string& message);
private:
    static void sendAssertSignal();
};

#define CHECK_MSG(condition, message) AssertUtils::checkMsg((condition), #condition, __FILE__, __LINE__, __FUNCTION__, message);
