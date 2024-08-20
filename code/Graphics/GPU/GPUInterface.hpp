#pragma once

#include "Core/Minimal.hpp"
#include "Core/ECS/System.hpp"
// #include "//glew-2.2.0/include/GL///glew.h"
#include "vulkan/vulkan.hpp"

class Camera;

enum class GPUBufferType : u32
{
    VERTEX /*= GL_ARRAY_BUFFER*/,
    INDEX /*= GL_ELEMENT_ARRAY_BUFFER*/,
    UNIFORM /*= GL_UNIFORM_BUFFER*/,
    STORAGE /*= GL_SHADER_STORAGE_BUFFER*/
};

enum class GPUDrawPrimitive: u32
{
    LINES /*= GL_LINES*/,
    TRIANGLES /*= GL_TRIANGLES*/
};

enum class GPUFramebufferAttachmentType: u32
{
    NONE /*= GL_NONE*/,
    COLOR0 /*= GL_COLOR_ATTACHMENT0*/,
    COLOR1 /*= GL_COLOR_ATTACHMENT1*/,
    COLOR2 /*= GL_COLOR_ATTACHMENT2*/,
    COLOR3 /*= GL_COLOR_ATTACHMENT3*/,
    COLOR4 /*= GL_COLOR_ATTACHMENT4*/,
    COLOR5 /*= GL_COLOR_ATTACHMENT5*/,
    COLOR6 /*= GL_COLOR_ATTACHMENT6*/,
    COLOR7 /*= GL_COLOR_ATTACHMENT7*/,
    COLOR8 /*= GL_COLOR_ATTACHMENT8*/,
    COLOR9 /*= GL_COLOR_ATTACHMENT9*/,
    COLOR10 /*= GL_COLOR_ATTACHMENT10*/,
    COLOR11 /*= GL_COLOR_ATTACHMENT11*/,
    COLOR12 /*= GL_COLOR_ATTACHMENT12*/,
    COLOR13 /*= GL_COLOR_ATTACHMENT13*/,
    COLOR14 /*= GL_COLOR_ATTACHMENT14*/,
    COLOR15 /*= GL_COLOR_ATTACHMENT15*/,
    DEPTH /*= GL_DEPTH_ATTACHMENT*/,
    STENCIL /*= GL_STENCIL_ATTACHMENT*/,
    DEPTH_STENCIL /*= GL_DEPTH_STENCIL_ATTACHMENT*/
};

enum class GPUFramebufferOperationType: u32
{
    READ /*= GL_READ_FRAMEBUFFER*/,
    DRAW /*= GL_DRAW_FRAMEBUFFER*/,
    READ_AND_DRAW /*= GL_FRAMEBUFFER*/
};

// aka internal format in OpenGL
enum class GPUTextureFormat: u32
{
    // Sized
    DEPTH_COMPONENT32F /*= GL_DEPTH_COMPONENT32F*/,
    DEPTH_COMPONENT24 /*= GL_DEPTH_COMPONENT24*/,
    DEPTH_COMPONENT16 /*= GL_DEPTH_COMPONENT16*/,
    DEPTH32F_STENCIL8 /*= GL_DEPTH32F_STENCIL8*/,
    DEPTH24_STENCIL8 /*= GL_DEPTH24_STENCIL8*/,
    STENCIL_INDEX8 /*= GL_STENCIL_INDEX8*/,
    R8 /*= GL_R8*/,
    R8_SNORM /*= GL_R8_SNORM*/,
    R16 /*= GL_R16*/,
    R16_SNORM /*= GL_R16_SNORM*/,
    RG8 /*= GL_RG8*/,
    RG8_SNORM /*= GL_RG8_SNORM*/,
    RG16 /*= GL_RG16*/,
    RG16_SNORM /*= GL_RG16_SNORM*/,
    RGB4 /*= GL_RGB4*/,
    RGB8 /*= GL_RGB8*/,
    RGB8_SNORM /*= GL_RGB8_SNORM*/,
    RGB16_SNORM /*= GL_RGB16_SNORM*/,
    RGBA8 /*= GL_RGBA8*/,
    RGBA8_SNORM /*= GL_RGBA8_SNORM*/,
    RGBA16 /*= GL_RGBA16*/,
    SRGB8 /*= GL_SRGB8*/,
    R16F /*= GL_R16F*/,
    RG16F /*= GL_RG16F*/,
    RGB16F /*= GL_RGB16F*/,
    RGBA16F /*= GL_RGBA16F*/,
    R32F /*= GL_R32F*/,
    RG32F /*= GL_RG32F*/,
    RGB32F /*= GL_RGB32F*/,
    RGBA32F /*= GL_RGBA32F*/,
    R8I /*= GL_R8I*/,
    R8UI /*= GL_R8UI*/,
    R16I /*= GL_R16I*/,
    R16UI /*= GL_R16UI*/,
    R32I /*= GL_R32I*/,
    R32UI /*= GL_R32UI*/,
    RG8I /*= GL_RG8I*/,
    RG8UI /*= GL_RG8UI*/,
    RG16I /*= GL_RG16I*/,
    RG16UI /*= GL_RG16UI*/,
    RG32I /*= GL_RG32I*/,
    RG32UI /*= GL_RG32UI*/,
    RGB8I /*= GL_RGB8I*/,
    RGB8UI /*= GL_RGB8UI*/,
    RGB16I /*= GL_RGB16I*/,
    RGB16UI /*= GL_RGB16UI*/,
    RGB32I /*= GL_RGB32I*/,
    RGB32UI /*= GL_RGB32UI*/,
    RGBA8I /*= GL_RGBA8I*/,
    RGBA8UI /*= GL_RGBA8UI*/,
    RGBA16I /*= GL_RGBA16I*/,
    RGBA16UI /*= GL_RGBA16UI*/,
    RGBA32I /*= GL_RGBA32I*/,
    RGBA32UI /*= GL_RGBA32UI*/
};

enum class GPUTexturePixelFormat: u32
{
    RED /*= GL_RED*/,
    RGB /*= GL_RGB*/,
    RGBA /*= GL_RGBA*/,
    DEPTH_COMPONENT /*= GL_DEPTH_COMPONENT*/,
    DEPTH_STENCIL /*= GL_DEPTH_STENCIL*/,
    STENCIL_INDEX /*= GL_STENCIL_INDEX*/
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
    UNSIGNED_BYTE /*= GL_UNSIGNED_BYTE*/,
    BYTE /*= GL_BYTE*/,
    UNSIGNED_SHORT /*= GL_UNSIGNED_SHORT*/,
    SHORT /*= GL_SHORT*/,
    UNSIGNED_INT /*= GL_UNSIGNED_INT*/,
    UNSIGNED_INT_24_8 /*= GL_UNSIGNED_INT_24_8*/,
    INT /*= GL_INT*/,
    HALF_FLOAT /*= GL_HALF_FLOAT*/,
    FLOAT /*= GL_FLOAT*/,
    BOOL /*= GL_BOOL*/
};

enum class GPUStencilFunction : u32
{
    NEVER /*= GL_NEVER*/,
    ALWAYS /*= GL_ALWAYS*/,
    LESS /*= GL_LESS*/,
    LEQUAL /*= GL_LEQUAL*/,
    GREATER /*= GL_GREATER*/,
    GEQUAL /*= GL_GEQUAL*/,
    EQUAL /*= GL_EQUAL*/,
    NOTEQUAL /*= GL_NOTEQUAL*/
};

enum class GPUStencilOp : u32
{
    KEEP /*= GL_KEEP*/,
    INVERT /*= GL_INVERT*/,
    ZERO /*= GL_ZERO*/,
    REPLACE /*= GL_REPLACE*/,
    INCR /*= GL_INCR*/,
    INCR_WRAP /*= GL_INCR_WRAP*/,
    DECR /*= GL_DECR*/,
    DECR_WRAP /*= GL_DECR_WRAP*/
};

enum class GPUBlendFactor : u32
{
    ZERO /*= GL_ZERO*/,
    ONE /*= GL_ONE*/,
    SRC_COLOR /*= GL_SRC_COLOR*/,
    ONE_MINUS_SRC_COLOR /*= GL_ONE_MINUS_SRC_COLOR*/,
    DST_COLOR /*= GL_DST_COLOR*/,
    ONE_MINUS_DST_COLOR /*= GL_ONE_MINUS_DST_COLOR*/,
    SRC_ALPHA /*= GL_SRC_ALPHA*/,
    ONE_MINUS_SRC_ALPHA /*= GL_ONE_MINUS_SRC_ALPHA*/,
    DST_ALPHA /*= GL_DST_ALPHA*/,
    ONE_MINUS_DST_ALPHA /*= GL_ONE_MINUS_DST_ALPHA*/,
    CONSTANT_COLOR /*= GL_CONSTANT_COLOR*/,
    ONE_MINUS_CONSTANT_COLOR /*= GL_ONE_MINUS_CONSTANT_COLOR*/,
    CONSTANT_ALPHA /*= GL_CONSTANT_ALPHA*/, 
    ONE_MINUS_CONSTANT_ALPHA /*= GL_ONE_MINUS_CONSTANT_ALPHA*/
};

enum class GPUDepthFunc : u32
{
    NEVER /*= GL_NEVER*/,
    LESS /*= GL_LESS*/,
    EQUAL /*= GL_EQUAL*/,
    LEQUAL /*= GL_LEQUAL*/,
    GREATER /*= GL_GREATER*/,
    NOTEQUAL /*= GL_NOTEQUAL*/,
    GEQUAL /*= GL_GEQUAL*/,
    ALWAYS /*= GL_ALWAYS*/
};

enum class GPUFlags : u32
{
    MULTISAMPLE /*= GL_MULTISAMPLE*/,
    DEPTH_TEST /*= GL_DEPTH_TEST*/,
    BLEND /*= GL_BLEND*/,
    CULL_FACE /*= GL_CULL_FACE*/
};

DECLARE_ENUM(GPUCullFaceType,
    NONE = 0, "NONE",
    FRONT, "FRONT",
    BACK, "BACK",
    FRONT_AND_BACK, "FRONT_AND_BACK"
);

#define TO_U32(x) static_cast<u32>(x)

using TextureHandle = u64;