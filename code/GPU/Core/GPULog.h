#pragma once

#include "Core/Log/Log.hpp"

#define VULKAN_LOG(...) LOG_TAG("VULKAN", __VA_ARGS__)
#define VULKAN_LOG_TAG(tag, ...) LOG_TAG("VULKAN " + std::string(tag), __VA_ARGS__)
#define VULKAN_LOG_ERROR(...) VULKAN_LOG_TAG("ERROR", __VA_ARGS__)
#define VULKAN_LOG_WARNING(...) VULKAN_LOG_TAG("WARNING", __VA_ARGS__)