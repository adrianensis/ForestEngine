#pragma once

#include "Core/Log/Log.hpp"

#define GPU_LOG(...) LOG_TAG("GPU", __VA_ARGS__)
#define GPU_LOG_TAG(tag, ...) LOG_TAG("GPU " + std::string(tag), __VA_ARGS__)
#define GPU_LOG_ERROR(...) GPU_LOG_TAG("ERROR", __VA_ARGS__)
#define GPU_LOG_WARNING(...) GPU_LOG_TAG("WARNING", __VA_ARGS__)