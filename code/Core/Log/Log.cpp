#include "Core/Log/Log.hpp"

NS_BEGIN(Core)
void Log::init()
{
	logFile.open("log.txt");
}

void Log::terminate()
{
	if(logFile.is_open())
	{
		logFile.close();
	}
}
NS_END