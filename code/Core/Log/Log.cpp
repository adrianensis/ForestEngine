#include "Core/Log/Log.hpp"

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