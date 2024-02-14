#pragma once

#include "Core/StdCore.hpp"
#include <fstream>

class Log
{
public:
	inline static const std::string_view emptyMessage;
	inline static std::ofstream logFile;

    class Prefixes
    {
    public:
        inline static const std::string smLog = "Log";
        inline static const std::string smVar = "Var";
        inline static const std::string smTrace = "Trace";
        inline static const std::string smError = "Error";
        inline static const std::string smAssert = "Assert";
    };

	Log() = default;
	~Log() = default;

	template <class T>
	static void var(const std::string_view& varname, T var)
	{
		std::string valueStr;
		if constexpr (std::is_same<T, std::string_view>::value)
		{
			valueStr = var;
		}
		else
		{
			valueStr = std::to_string(var);
		}

		log(Prefixes::smVar, std::string(varname) + " : " + valueStr, true);
	};

	template <class T>
	static void val(const T& var)
	{
		std::string valueStr;
		if constexpr (std::is_same<T, std::string_view>::value)
		{
			valueStr = var;
		}
		else
		{
			valueStr = std::to_string(var);
		}

		log(Prefixes::smLog, valueStr, true);
	};

    static void init();
    static void terminate();
    static void trace(const std::string_view file, u32 line, const std::string_view function, const std::string_view message = emptyMessage);
    static void log(const std::string_view& tag, const std::string_view& message, bool newLine);
    static void brline();
    static void backspace();

private:
    static void writeLine(const std::string_view& str);
    static void append(const std::string_view& str);
};

#ifdef ENGINE_ENABLE_LOGS
#define LOG_TRACE() Log::trace(__FILE__, __LINE__, __PRETTY_FUNCTION__);
#define LOG(x) Log::log(Log::Prefixes::smLog, x, true);
#define LOG_APPEND(x) Log::log(Log::Prefixes::smLog, x, false);
#define LOG_TAG(Tag, x) Log::log(Tag, x, true);
#define LOG_TAG_APPEND(Tag, x) Log::log(Tag, x, false);
#define LOG_VAR(x) Log::var<REMOVE_POINTER(REMOVE_REF(decltype(x)))>(#x, x);
#define LOG_VAL(x) Log::val<REMOVE_POINTER(REMOVE_REF(decltype(x)))>(x);
#define LOG_ERROR(x) Log::log(Log::Prefixes::smError, x, true);
#define LOG_BRLINE() Log::brline();
#define LOG_BACKSPACE() Log::backspace();
#else
#define LOG_TRACE()
#define LOG(x)
#define LOG_APPEND(x)
#define LOG_TAG(Tag, x)
#define LOG_TAG_APPEND(Tag, x)
#define LOG_VAR(x)
#define LOG_VAL(x)
#define LOG_ERROR(x)
#define LOG_BRLINE()
#define LOG_BACKSPACE()
#endif
