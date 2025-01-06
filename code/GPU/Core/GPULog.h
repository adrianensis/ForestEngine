#pragma once

#include "Core/Log/Log.hpp"

#define VULKAN_LOG(x) LOG_TAG("VULKAN", x)
#define VULKAN_LOG_TAG(tag, x) LOG_TAG("VULKAN " + std::string(tag), x)
#define VULKAN_LOG_ERROR(x) VULKAN_LOG_TAG("ERROR", x)
#define VULKAN_LOG_WARNING(x) VULKAN_LOG_TAG("WARNING", x)