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

void Log::log(const std::string_view& str)
{
	std::cout << str << std::endl;
	logFile << str << std::endl;
}

void Log::append(const std::string_view& str)
{
	std::cout << '\r';
	std::cout << str;
	logFile << str;
	std::cout.flush();
}

void Log::trace(const std::string_view file, u32 line, const std::string_view function, const std::string_view message /*= emptyMessage*/)
{
	log("TRACE > [" + std::string(function) + ":" + std::to_string(line) + "] > " + std::string(message));
}

void Log::echo(const std::string_view& message, bool newLine /*= true*/)
{
	if(newLine)
	{
		log("ECHO > " + std::string(message));
	}
	else
	{
		append("ECHO > " + std::string(message));
	}
}

void Log::customEcho(const std::string_view& tag, const std::string_view& message, bool newLine /*= true*/)
{
	if(newLine)
	{
		log(std::string(tag) + " > " + std::string(message));
	}
	else
	{
		append(std::string(tag) + " > " + std::string(message));
	}
}

void Log::error(const std::string_view& message)
{
	log("ERROR > " + std::string(message));
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
