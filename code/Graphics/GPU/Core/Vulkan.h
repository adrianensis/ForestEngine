#pragma once

#include <vulkan/vulkan.h>
#include "vulkan/vulkan.hpp"

#include "Core/Minimal.hpp"

#define VULKAN_LOG(x) LOG_TAG("VULKAN", x)
#define VULKAN_LOG_TAG(tag, x) LOG_TAG("VULKAN " + std::string(tag), x)
#define VULKAN_LOG_ERROR(x) VULKAN_LOG_TAG("ERROR", x)
#define VULKAN_LOG_WARNING(x) VULKAN_LOG_TAG("WARNING", x)

class Camera;

enum class GPUBufferType : u32
{
    VERTEX,
    INDEX,
    UNIFORM,
    STORAGE
};

enum class GPUDrawPrimitive: u32
{
    LINES,
    TRIANGLES
};

enum class GPUFramebufferAttachmentType: u32
{
    NONE,
    COLOR0,
    COLOR1,
    COLOR2,
    COLOR3,
    COLOR4,
    COLOR5,
    COLOR6,
    COLOR7,
    COLOR8,
    COLOR9,
    COLOR10,
    COLOR11,
    COLOR12,
    COLOR13,
    COLOR14,
    COLOR15,
    DEPTH,
    STENCIL,
    DEPTH_STENCIL
};

enum class GPUFramebufferOperationType: u32
{
    READ,
    DRAW,
    READ_AND_DRAW
};

// aka internal format in OpenGL
enum class GPUTextureFormat: u32
{
    // Sized
    DEPTH_COMPONENT32F,
    DEPTH_COMPONENT24,
    DEPTH_COMPONENT16,
    DEPTH32F_STENCIL8,
    DEPTH24_STENCIL8,
    STENCIL_INDEX8,
    R8,
    R8_SNORM,
    R16,
    R16_SNORM,
    RG8,
    RG8_SNORM,
    RG16,
    RG16_SNORM,
    RGB4,
    RGB8,
    RGB8_SNORM,
    RGB16_SNORM,
    RGBA8,
    RGBA8_SNORM,
    RGBA16,
    SRGB8,
    R16F,
    RG16F,
    RGB16F,
    RGBA16F,
    R32F,
    RG32F,
    RGB32F,
    RGBA32F,
    R8I,
    R8UI,
    R16I,
    R16UI,
    R32I,
    R32UI,
    RG8I,
    RG8UI,
    RG16I,
    RG16UI,
    RG32I,
    RG32UI,
    RGB8I,
    RGB8UI,
    RGB16I,
    RGB16UI,
    RGB32I,
    RGB32UI,
    RGBA8I,
    RGBA8UI,
    RGBA16I,
    RGBA16UI,
    RGBA32I,
    RGBA32UI
};

enum class GPUTexturePixelFormat: u32
{
    RED,
    RGB,
    RGBA,
    DEPTH_COMPONENT,
    DEPTH_STENCIL,
    STENCIL_INDEX
};

enum class GPUPipelineStage: u32
{
    NONE,
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    TESS_CONTROL,
    TESS_EVALUATION,
    COMPUTE
};

enum class GPUPrimitiveDataType : u32
{
    VOID,
    STRUCT,
    UNSIGNED_BYTE,
    BYTE,
    UNSIGNED_SHORT,
    SHORT,
    UNSIGNED_INT,
    UNSIGNED_INT_24_8,
    INT,
    HALF_FLOAT,
    FLOAT,
    BOOL
};

enum class GPUStencilFunction : u32
{
    NEVER,
    ALWAYS,
    LESS,
    LEQUAL,
    GREATER,
    GEQUAL,
    EQUAL,
    NOTEQUAL
};

enum class GPUStencilOp : u32
{
    KEEP,
    INVERT,
    ZERO,
    REPLACE,
    INCR,
    INCR_WRAP,
    DECR,
    DECR_WRAP
};

enum class GPUBlendFactor : u32
{
    ZERO,
    ONE,
    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,
    DST_COLOR,
    ONE_MINUS_DST_COLOR,
    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA,
    CONSTANT_COLOR,
    ONE_MINUS_CONSTANT_COLOR,
    CONSTANT_ALPHA, 
    ONE_MINUS_CONSTANT_ALPHA
};

enum class GPUDepthFunc : u32
{
    NEVER,
    LESS,
    EQUAL,
    LEQUAL,
    GREATER,
    NOTEQUAL,
    GEQUAL,
    ALWAYS
};

enum class GPUFlags : u32
{
    MULTISAMPLE,
    DEPTH_TEST,
    BLEND,
    CULL_FACE
};

DECLARE_ENUM(GPUCullFaceType,
    NONE = 0, "NONE",
    FRONT, "FRONT",
    BACK, "BACK",
    FRONT_AND_BACK, "FRONT_AND_BACK"
);

#define TO_U32(x) static_cast<u32>(x)

using TextureHandle = u64;

class GPUWindow;

class Vulkan {
public:
    struct Config {
        std::string Name;
        uint32_t MajorVersion = 1;
        uint32_t MinorVersion = 0;
        uint32_t PatchVersion = 0;
        bool ValidationLayersEnabled = true;
    };

public:
    Vulkan(Config config, GPUWindow* gpuWindow);

    VkInstance getGPUInstance() const;

    VkSurfaceKHR getSurface() const;

    const std::vector<const char*>& getValidationLayers() const;

    bool isValidationLayersEnabled() const;

    bool initialize();

    void terminate();

private:
    bool createInstance();
    void destroyInstance();
    bool createDebugMessenger();
    void destroyDebugMessenger();
    bool createSurface() const;
    void destroySurface() const;
    std::vector<const char*> findExtensions() const;
    std::vector<const char*> findRequiredExtensions() const;
    std::vector<VkExtensionProperties> findAvailableExtensions() const;
    bool hasExtensions(const std::vector<const char*>& extensions, const std::vector<VkExtensionProperties>& availableExtensions) const;
    std::vector<const char*> findValidationLayers() const;
    std::vector<VkLayerProperties> findAvailableValidationLayers() const;
    bool hasValidationLayers(const std::vector<const char*>& validationLayers, const std::vector<VkLayerProperties>& availableValidationLayers) const;
    VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo() const;

private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

private:
    Config config;
    GPUWindow* gpuWindow;
    std::vector<const char*> validationLayers{};
    VkInstance vulkanInstance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
};