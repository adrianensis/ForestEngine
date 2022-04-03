#pragma once

#include "Core/BasicTypes.hpp"

#ifdef CPP_INCLUDE
#include "Core/Log/Log.hpp"
#include "Core/BasicTypes.hpp"
#endif

class Log
{
public:
	inline static const std::string emptyMessage;
	inline static std::ofstream logFile;

	Log() = default;
	~Log() = default;

	template <class T>
	static void var(const std::string& varname, T var)
	{
		std::string valueStr;
		if constexpr (std::is_same<T, std::string>::value)
		{
			valueStr = var;
		}
		else
		{
			valueStr = std::to_string(var);
		}

		log("VAR > " + varname + " : " + valueStr);
	};

	template <class T>
	static void val(const T& var)
	{
		std::string valueStr;
		if constexpr (std::is_same<T, std::string>::value)
		{
			valueStr = var;
		}
		else
		{
			valueStr = std::to_string(var);
		}

		log("VAL > " + valueStr);
	};

	CPP static void init()
	{
		logFile.open("log.txt");
	}

	CPP static void terminate()
	{
		logFile.close();
	}

	CPP static void log(const std::string& str)
	{
		std::cout << str << std::endl;
		logFile << str << std::endl;
	}

	CPP static void append(const std::string& str)
	{
		std::cout << '\r';
		std::cout << str;
		logFile << str;
		std::cout.flush();
	}

	CPP static void trace(const std::string file, u32 line, const std::string function, const std::string message = emptyMessage)
	{
		// std::cout << "TRACE > [" /* << std::experimental::filesystem::path(file).filename() << ":" */ << function << ":" << std::to_string(line) << "] > " << message << std::endl;
		log("TRACE > [" + function + ":" + std::to_string(line) + "] > " + message);
	}

	CPP static void echo(const std::string& message, bool newLine = true)
	{
		// std::cout << "ECHO > " << message << std::endl;
		if(newLine)
		{
			log("ECHO > " + message);
		}
		else
		{
			append("ECHO > " + message);
		}
	}

	CPP static void customEcho(const std::string& tag, const std::string& message, bool newLine = true)
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

	CPP static void error(const std::string& message)
	{
		// std::cout << "ERROR > " << message << std::endl;
		log("ERROR > " + message);
	}

	CPP static void brline()
	{ // break line
		log(emptyMessage);
	}

	CPP static void backspace()
	{
		std::cout << "\b \b";
		logFile << "\b \b";
	}
};

#ifdef DE_ENABLE_LOGS
#define TRACE() Log::trace(__FILE__, __LINE__, __PRETTY_FUNCTION__);
#define TO_STR(s) #s
#define ECHO(x) Log::echo(x);
#define ECHO_APPEND(x) Log::echo(x, false);
#define CUSTOM_ECHO(Tag, x) Log::customEcho(Tag, x);
#define CUSTOM_ECHO_APPEND(Tag, x) Log::customEcho(Tag, x, false);
#define VAR(x) Log::var<REMOVE_POINTER(REMOVE_REF(decltype(x)))>(#x, x);
#define VAL(x) Log::val<REMOVE_POINTER(REMOVE_REF(decltype(x)))>(x);
#define ERROR(x) Log::error(x);
#define BRLINE() Log::brline();
#define BACKSPACE() Log::backspace();
#else
#define TRACE()
#define TO_STR(s)
#define ECHO(x)
#define ECHO_APPEND(x)
#define CUSTOM_ECHO(Tag, x)
#define CUSTOM_ECHO_APPEND(Tag, x)
#define VAR(x)
#define VAL(x)
#define ERROR(x)
#define BRLINE()
#define BACKSPACE()
#endif
