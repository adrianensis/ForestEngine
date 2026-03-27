#include "Core/Log/Log.hpp"

NS_BEGIN(Core)
void Log::init(const std::string& logDirectory)
{
	logFile.open(logDirectory+smLogFile);
}

void Log::terminate()
{
	if(logFile.is_open())
	{
		logFile.close();
	}
}
NS_END