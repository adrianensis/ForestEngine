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

void Log::log(const std::string& str)
{
	std::cout << str << std::endl;
	logFile << str << std::endl;
}

void Log::append(const std::string& str)
{
	std::cout << '\r';
	std::cout << str;
	logFile << str;
	std::cout.flush();
}

void Log::trace(const std::string file, u32 line, const std::string function, const std::string message /*= emptyMessage*/)
{
	log("TRACE > [" + function + ":" + std::to_string(line) + "] > " + message);
}

void Log::echo(const std::string& message, bool newLine /*= true*/)
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

void Log::customEcho(const std::string& tag, const std::string& message, bool newLine /*= true*/)
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

void Log::error(const std::string& message)
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
