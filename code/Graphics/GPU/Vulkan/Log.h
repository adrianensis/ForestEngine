#pragma once

#include "Environment.h"

#ifndef __FILE_NAME__
    #define __FILE_NAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

#define VD_FILENAME __FILE_NAME__
#define VD_FUNCTION_NAME __func__
#define VD_LINE_NUMBER __LINE__

#define VD_FORMAT_LOG_MESSAGE(message)

#define VD_LOG_TRACE(message, ...)
#define VD_LOG_DEBUG(message, ...)
#define VD_LOG_INFO(message, ...)
#define VD_LOG_WARN(message, ...)
#define VD_LOG_ERROR(message, ...)
#define VD_LOG_CRITICAL(message, ...)

namespace GPUAPI {

    class Log {
    public:
        enum class Level {
            None = 0,
            Trace,
            Debug,
            Info,
            Warn,
            Error,
            Critical
        };
    };
}