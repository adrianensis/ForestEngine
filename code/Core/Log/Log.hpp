#pragma once

#include "Core/StdCore.hpp"
#include <fstream>
#include <ostream>
#include "fmt/core.h"
#include "fmt/base.h"
#include "fmt/ostream.h"

NS_BEGIN(Core)
class Log
{
public:
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

		log(Prefixes::smVar, true, std::string(varname) + " : " + valueStr);
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

		log(Prefixes::smLog, true, valueStr);
	};

    static void init();
    static void terminate();
    static void brline()
	{
		// break line
		writeLine("");
	}

	static void backspace()
	{
		append("\b \b");
	}

	template <typename... T>
	static void log(const std::string_view& tag, bool newLine, const std::string_view& fmt, T&&... args)
	{
		if(newLine)
		{
			writeLine("{} > "s + std::string(fmt), std::string(tag), args...);
		}
		else
		{
			append("{} > "s + std::string(fmt), std::string(tag), args...);
		}
	}

	static void trace(const std::string_view file, u32 line, const std::string_view function, const std::string& message)
	{
		log(Prefixes::smTrace, true, "[{}:{}] {}", std::string(function), std::to_string(line), message);
	}

private:
	template <typename... T>
	static void writeLine(const std::string_view& fmt, T&&... args)
	{
		fmt::println(fmt::runtime(fmt), args...);
		fmt::println(logFile, fmt::runtime(fmt), args...);
		std::flush(logFile);
	}

	template <typename... T>
	static void append(const std::string_view& fmt, T&&... args)
	{
		fmt::print(fmt::runtime(fmt), args...);
		fmt::print(logFile, fmt::runtime(fmt), args...);
		std::flush(logFile);
	}
};

#ifdef ENGINE_ENABLE_LOGS
#define LOG_TRACE() Core::Log::trace(__FILE__, __LINE__, __PRETTY_FUNCTION__, ""s);
#define LOG_TRACE_MSG(...) Core::Log::trace(__FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__);
#define LOG(...) Core::Log::log(Core::Log::Prefixes::smLog, true, __VA_ARGS__);
#define LOG_APPEND(...) Core::Log::log(Core::Log::Prefixes::smLog, false, __VA_ARGS__);
#define LOG_TAG(Tag, ...) Core::Log::log(Tag, true, __VA_ARGS__);
#define LOG_TAG_APPEND(Tag, ...) Core::Log::log(Tag, false, __VA_ARGS__);
#define LOG_VAR(x) Core::Log::var<REMOVE_POINTER(REMOVE_REFERENCE(decltype(x)))>(#x, x);
#define LOG_VAL(x) Core::Log::val<REMOVE_POINTER(REMOVE_REFERENCE(decltype(x)))>(x);
#define LOG_ERROR(...) Core::Log::log(Core::Log::Prefixes::smError, true, __VA_ARGS__);
#define LOG_BRLINE() Core::Log::brline();
#define LOG_BACKSPACE() Core::Log::backspace();
#else
#define LOG_TRACE()
#define LOG_TRACE_MSG(...)
#define LOG(...)
#define LOG_APPEND(...)
#define LOG_TAG(Tag, ...)
#define LOG_TAG_APPEND(Tag, ...)
#define LOG_VAR(x)
#define LOG_VAL(x)
#define LOG_ERROR(...)
#define LOG_BRLINE()
#define LOG_BACKSPACE()
#endif
NS_END