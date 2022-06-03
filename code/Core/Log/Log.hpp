#ifndef LOG_HPP
#define LOG_HPP

#include "Core/BasicTypes.hpp"

class Log
{
public:
	inline static const String emptyMessage;
	inline static std::ofstream logFile;

	Log() = default;
	~Log() = default;

	template <class T>
	static void var(const String& varname, T var)
	{
		String valueStr;
		if constexpr (std::is_same<T, String>::value)
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
		String valueStr;
		if constexpr (std::is_same<T, String>::value)
		{
			valueStr = var;
		}
		else
		{
			valueStr = std::to_string(var);
		}

		log("VAL > " + valueStr);
	};

    static void init();
    static void terminate();
    static void log(const String& str);
    static void append(const String& str);
    static void trace(const String file, u32 line, const String function, const String message = emptyMessage);
    static void echo(const String& message, bool newLine = true);
    static void customEcho(const String& tag, const String& message, bool newLine = true);
    static void error(const String& message);
    static void brline();
    static void backspace();
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

#endif