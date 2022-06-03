#include "Core/Log/Log.hpp"
#include "Core/BasicTypes.hpp"


void Log::init()
{
	logFile.open("log.txt");
}

void Log::terminate()
{
	logFile.close();
}

void Log::log(const String& str)
{
	std::cout << str << std::endl;
	logFile << str << std::endl;
}

void Log::append(const String& str)
{
	std::cout << '\r';
	std::cout << str;
	logFile << str;
	std::cout.flush();
}

void Log::trace(const String file, u32 line, const String function, const String message /*= emptyMessage*/)
{
	log("TRACE > [" + function + ":" + std::to_string(line) + "] > " + message);
}

void Log::echo(const String& message, bool newLine /*= true*/)
{
	if(newLine)
	{
		log("ECHO > " + message);
	}
	else
	{
		append("ECHO > " + message);
	}
}

void Log::customEcho(const String& tag, const String& message, bool newLine /*= true*/)
{
	if(newLine)
	{
		log(tag + " > " + message);
	}
	else
	{
		append(tag + " > " + message);
	}
}

void Log::error(const String& message)
{
	log("ERROR > " + message);
}

void Log::brline()
{ // break line
	log(emptyMessage);
}

void Log::backspace()
{
	std::cout << "\b \b";
	logFile << "\b \b";
}
