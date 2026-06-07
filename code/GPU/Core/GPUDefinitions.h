#pragma once

#include "Core/Core.hpp"
#include <cstdint>
#include <vulkan/vulkan.h>

#if defined(_WIN64)
	#define GPU_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define GPU_PLATFORM_MACOS
/* check for __android__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined
 */
#elif defined(__ANDROID__)
    #define GPU_PLATFORM_ANDROID
#elif defined(__linux__)
    #define GPU_PLATFORM_LINUX
#else
	#error "Unsupported platform"
#endif

namespace GPU
{
    using f32 = float;
    using f64 = double;

    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using byte = std::uint8_t;
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
};

enum class GPUBufferType : GPU::u32
{
    VERTEX,
    INDEX,
    UNIFORM,
    STORAGE
};

enum class GPUDrawPrimitive: GPU::u32
{
    LINES,
    TRIANGLES
};

enum class GPUFramebufferAttachmentType: GPU::u32
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

enum class GPUFramebufferOperationType: GPU::u32
{
    READ,
    DRAW,
    READ_AND_DRAW
};

// aka internal format in OpenGL
enum class GPUTextureFormat: GPU::u32
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

enum class GPUTexturePixelFormat: GPU::u32
{
    RED,
    RGB,
    RGBA,
    DEPTH_COMPONENT,
    DEPTH_STENCIL,
    STENCIL_INDEX
};

enum class GPUPipelineStage: GPU::u32
{
    NONE,
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    TESS_CONTROL,
    TESS_EVALUATION,
    COMPUTE
};

enum class GPUPrimitiveDataType : GPU::u32
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

enum class GPUStencilFunction : GPU::u32
{
    NEVER = VK_COMPARE_OP_NEVER,
    ALWAYS = VK_COMPARE_OP_ALWAYS,
    LESS = VK_COMPARE_OP_LESS,
    LEQUAL = VK_COMPARE_OP_LESS_OR_EQUAL,
    GREATER = VK_COMPARE_OP_GREATER,
    GEQUAL = VK_COMPARE_OP_GREATER_OR_EQUAL,
    EQUAL = VK_COMPARE_OP_EQUAL,
    NOTEQUAL = VK_COMPARE_OP_NOT_EQUAL
};

enum class GPUStencilOp : GPU::u32
{
    KEEP = VK_STENCIL_OP_KEEP,
    INVERT = VK_STENCIL_OP_INVERT,
    ZERO = VK_STENCIL_OP_ZERO,
    REPLACE = VK_STENCIL_OP_REPLACE,
    INCR = VK_STENCIL_OP_INCREMENT_AND_CLAMP,
    INCR_WRAP = VK_STENCIL_OP_INCREMENT_AND_WRAP,
    DECR = VK_STENCIL_OP_DECREMENT_AND_CLAMP,
    DECR_WRAP = VK_STENCIL_OP_DECREMENT_AND_WRAP 
};

enum class GPUCompareOp : GPU::u32
{
    NEVER = VK_COMPARE_OP_NEVER,
    LESS = VK_COMPARE_OP_LESS,
    EQUAL = VK_COMPARE_OP_EQUAL,
    LESS_OR_EQUAL = VK_COMPARE_OP_LESS_OR_EQUAL,
    GREATER = VK_COMPARE_OP_GREATER,
    NOT_EQUAL = VK_COMPARE_OP_NOT_EQUAL,
    GREATER_OR_EQUAL = VK_COMPARE_OP_GREATER_OR_EQUAL,
    ALWAYS = VK_COMPARE_OP_ALWAYS
};

enum class GPUBlendFactor : GPU::u32
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

enum class GPUDepthFunc : GPU::u32
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

enum class GPUFlags : GPU::u32
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


class GPUConstants
{
public:
    inline static const GPU::u32 MAX_BONE_INFLUENCE = 4;
    inline static const GPU::u32 MAX_BONES = 65;
};

class GPUDepthStencilData
{
public:
    bool mStencilEnable = false;
    bool mDepthTestEnable = true;
    bool mDepthWriteEnable = true;
    GPUCompareOp mDepthCompareOp = GPUCompareOp::LESS;
    // aka ref
    GPU::u32 mStencilValue = 0;
    GPUStencilFunction mStencilFunction = GPUStencilFunction::NOTEQUAL;
    GPUStencilOp mStencilPassOp = GPUStencilOp::KEEP;
    GPUStencilOp mStencilFailOp = GPUStencilOp::KEEP;
    GPUStencilOp mDepthFailOp = GPUStencilOp::KEEP;
    GPU::u64 mParentId = 0;
    GPU::u64 mId = 0;

    bool operator==(const GPUDepthStencilData& other) const
    {
        if(this == &other) {return true;}
        return
        mStencilEnable == other.mStencilEnable &&
        mDepthTestEnable == other.mDepthTestEnable &&
        mDepthWriteEnable == other.mDepthWriteEnable &&
        mDepthCompareOp == other.mDepthCompareOp &&
        mStencilValue == other.mStencilValue &&
        mStencilFunction == other.mStencilFunction&&
        mStencilPassOp == other.mStencilPassOp &&
        mStencilFailOp == other.mStencilFailOp &&
        mDepthFailOp == other.mDepthFailOp &&
        mParentId == other.mParentId;
        //mId == other.mId && 
    }

    GPU::u64 hash() const
    {
        GPU::u32 shift = 0;
        GPU::u64 result = 0;
        result = result ^ static_cast<GPU::u64>(mStencilEnable) << (shift++);
        result = result ^ static_cast<GPU::u64>(mDepthTestEnable) << (shift++);
        result = result ^ static_cast<GPU::u64>(mDepthWriteEnable) << (shift++);
        result = result ^ static_cast<GPU::u64>(mDepthCompareOp) << (shift++);
        result = result ^ static_cast<GPU::u64>(mStencilValue) << (shift++);
        result = result ^ static_cast<GPU::u64>(mStencilFunction) << (shift++);
        result = result ^ static_cast<GPU::u64>(mStencilPassOp) << (shift++);
        result = result ^ static_cast<GPU::u64>(mStencilFailOp) << (shift++);
        result = result ^ static_cast<GPU::u64>(mDepthFailOp) << (shift++);
        result = result ^ static_cast<GPU::u64>(mParentId) << (shift++);
        return result;
    }
};

enum class GPUDescriptorSetScope : GPU::u32
{
    GLOBAL = 0,
    LOCAL = 1,
    MAX = 2
};

using GPURenderPassID = size_t;