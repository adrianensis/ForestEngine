#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "glew-2.2.0/include/GL/glew.h"

class Camera;

enum class GPUBufferType : u32
{
    VERTEX = GL_ARRAY_BUFFER,
    INDEX = GL_ELEMENT_ARRAY_BUFFER,
    UNIFORM = GL_UNIFORM_BUFFER,
    STORAGE = GL_SHADER_STORAGE_BUFFER
};

enum class GPUDrawPrimitive: u32
{
    LINES = GL_LINES,
    TRIANGLES = GL_TRIANGLES
};

enum class GPUFramebufferAttachmentType: u32
{
    NONE = GL_NONE,
    COLOR0 = GL_COLOR_ATTACHMENT0,
    COLOR1 = GL_COLOR_ATTACHMENT1,
    COLOR2 = GL_COLOR_ATTACHMENT2,
    COLOR3 = GL_COLOR_ATTACHMENT3,
    COLOR4 = GL_COLOR_ATTACHMENT4,
    COLOR5 = GL_COLOR_ATTACHMENT5,
    COLOR6 = GL_COLOR_ATTACHMENT6,
    COLOR7 = GL_COLOR_ATTACHMENT7,
    COLOR8 = GL_COLOR_ATTACHMENT8,
    COLOR9 = GL_COLOR_ATTACHMENT9,
    COLOR10 = GL_COLOR_ATTACHMENT10,
    COLOR11 = GL_COLOR_ATTACHMENT11,
    COLOR12 = GL_COLOR_ATTACHMENT12,
    COLOR13 = GL_COLOR_ATTACHMENT13,
    COLOR14 = GL_COLOR_ATTACHMENT14,
    COLOR15 = GL_COLOR_ATTACHMENT15,
    DEPTH = GL_DEPTH_ATTACHMENT,
    STENCIL = GL_STENCIL_ATTACHMENT,
    DEPTH_STENCIL = GL_DEPTH_STENCIL_ATTACHMENT
};

enum class GPUFramebufferOperationType: u32
{
    READ = GL_READ_FRAMEBUFFER,
    DRAW = GL_DRAW_FRAMEBUFFER,
    READ_AND_DRAW = GL_FRAMEBUFFER
};

// aka internal format in OpenGL
enum class GPUTextureFormat: u32
{
    // Sized
    DEPTH_COMPONENT32F = GL_DEPTH_COMPONENT32F,
    DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24,
    DEPTH_COMPONENT16 = GL_DEPTH_COMPONENT16,
    DEPTH32F_STENCIL8 = GL_DEPTH32F_STENCIL8,
    DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
    STENCIL_INDEX8 = GL_STENCIL_INDEX8,
    R8 = GL_R8,
    R8_SNORM = GL_R8_SNORM,
    R16 = GL_R16,
    R16_SNORM = GL_R16_SNORM,
    RG8 = GL_RG8,
    RG8_SNORM = GL_RG8_SNORM,
    RG16 = GL_RG16,
    RG16_SNORM = GL_RG16_SNORM,
    RGB4 = GL_RGB4,
    RGB8 = GL_RGB8,
    RGB8_SNORM = GL_RGB8_SNORM,
    RGB16_SNORM = GL_RGB16_SNORM,
    RGBA8 = GL_RGBA8,
    RGBA8_SNORM = GL_RGBA8_SNORM,
    RGBA16 = GL_RGBA16,
    SRGB8 = GL_SRGB8,
    R16F = GL_R16F,
    RG16F = GL_RG16F,
    RGB16F = GL_RGB16F,
    RGBA16F = GL_RGBA16F,
    R32F = GL_R32F,
    RG32F = GL_RG32F,
    RGB32F = GL_RGB32F,
    RGBA32F = GL_RGBA32F,
    R8I = GL_R8I,
    R8UI = GL_R8UI,
    R16I = GL_R16I,
    R16UI = GL_R16UI,
    R32I = GL_R32I,
    R32UI = GL_R32UI,
    RG8I = GL_RG8I,
    RG8UI = GL_RG8UI,
    RG16I = GL_RG16I,
    RG16UI = GL_RG16UI,
    RG32I = GL_RG32I,
    RG32UI = GL_RG32UI,
    RGB8I = GL_RGB8I,
    RGB8UI = GL_RGB8UI,
    RGB16I = GL_RGB16I,
    RGB16UI = GL_RGB16UI,
    RGB32I = GL_RGB32I,
    RGB32UI = GL_RGB32UI,
    RGBA8I = GL_RGBA8I,
    RGBA8UI = GL_RGBA8UI,
    RGBA16I = GL_RGBA16I,
    RGBA16UI = GL_RGBA16UI,
    RGBA32I = GL_RGBA32I,
    RGBA32UI = GL_RGBA32UI
};

enum class GPUTexturePixelFormat: u32
{
    RED = GL_RED,
    RGB = GL_RGB,
    RGBA = GL_RGBA,
    DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
    DEPTH_STENCIL = GL_DEPTH_STENCIL,
    STENCIL_INDEX = GL_STENCIL_INDEX
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
    UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
    BYTE = GL_BYTE,
    UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
    SHORT = GL_SHORT,
    UNSIGNED_INT = GL_UNSIGNED_INT,
    UNSIGNED_INT_24_8 = GL_UNSIGNED_INT_24_8,
    INT = GL_INT,
    HALF_FLOAT = GL_HALF_FLOAT,
    FLOAT = GL_FLOAT,
    BOOL = GL_BOOL
};

DECLARE_ENUM(GPUCullFaceType,
    NONE = 0, "NONE",
    FRONT = GL_FRONT, "FRONT",
    BACK = GL_BACK, "BACK",
    FRONT_AND_BACK = GL_FRONT_AND_BACK, "GL_FRONT_AND_BACK"
);

#define TO_U32(x) static_cast<u32>(x)

using TextureHandle = u64;

class GPUInterface : public System
{
public:

    bool loadAPI();

    template<class T>
    void getValue(u32 param, T& value)
    {
        if constexpr (std::is_same_v<f32, T>)
        {
            glGetFloatv(param, &value);
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            glGetDoublev(param, &value);
        }
        else if constexpr (std::is_same_v<bool, T>)
        {
            glGetBooleanv(param, &value);
        }
        else if constexpr (std::is_integral_v<T>)
        {
            glGetIntegerv(param, &value);
        }
        else
        {
            LOG_ERROR("Uniform type not supported!");
        }
    }

    // Vertex buffers layout
    u32 createVertexBufferLayout();
    void enableVertexBufferLayout(u32 vertexBufferLayout);
    void deleteVertexBufferLayout(u32 vertexBufferLayout);

    // Attribute
    void enableAttribute(u32 attributeIndex);
    void disableAttribute(u32 attributeIndex);
    void attribute(u32 propertyArrayIndex, u32 elementSize, GPUPrimitiveDataType primitiveType, u32 strideSize, u32 pointerOffset, u32 divisor);

    // Buffer
    u32 createBuffer();
    void bindBuffer(GPUBufferType bufferType, u32 bufferId);
    void resizeBuffer(GPUBufferType bufferType, u32 bufferId, u32 typeSizeInBytes, u32 size, bool isStatic);

    template<class T>
    void setBufferDataArray(GPUBufferType bufferType, u32 bufferId, const std::vector<T>& data)
    {
        setBufferDataRaw(bufferType, bufferId, sizeof(T), data.size(), data.data());
    }
    void setBufferDataArray(GPUBufferType bufferType, u32 bufferId, const ByteBuffer& data)
    {
        setBufferDataRaw(bufferType, bufferId, 1, data.sizeInBytes(), data.data());
    }

    template<class T>
    void setBufferData(GPUBufferType bufferType, u32 bufferId, const T& data)
    {
        setBufferDataRaw(bufferType, bufferId, sizeof(T), 1, &data);
    }

    void deleteBuffer(u32 bufferId);

    // Shared Buffer
    u32 getMaxElementsInSharedBuffer(GPUBufferType bufferType, u32 elementSizeInBytes);
    u32 getMaxBytesInSharedBuffer(GPUBufferType bufferType);
    u32 getMaxBindingPointsForSharedBuffer(GPUBufferType bufferType);
    void bindSharedBufferToBindingPoint(GPUBufferType bufferType, u32 bufferId, u32 bindingPoint);

    // Stencil
    void enableStencil(u32 stencilValue, u32 stencilFunction, u32 stencilPassOp);
    void disableStencil();

    // Texture
    u32 createTexture(GPUTextureFormat internalformat, u32 width, u32 height, bool createMipMap);
    u32 createTexture1ByteChannel(u32 width, u32 height, const byte* data);
    void setTextureStorage(u32 textureId, GPUTextureFormat internalformat, u32 width, u32 height);
    void setTextureData(u32 textureId, u32 width, u32 height, GPUTexturePixelFormat format, GPUPrimitiveDataType type, const byte* data);
    void setSubTexture(u32 textureId, u32 x, u32 y, u32 width, u32 height, GPUTexturePixelFormat format, GPUPrimitiveDataType type, const byte* data);
    void deleteTexture(u32 textureId);
    void enableTexture(u32 textureId, u32 textureUnit, GPUPipelineStage stage);
    void disableTexture(u32 textureUnit, GPUPipelineStage stage);
    void setPixelStoreMode(u32 param, u32 value);
    u32 getMaxTextureUnits(GPUPipelineStage stage);

    TextureHandle getTextureHandle(u32 texture​Id);
    void makeTextureResident(TextureHandle handle​, bool makeResident);

    template<class T>
    void setTextureParameter(u32 textureId, u32 param, const T& value)
    {
        if constexpr (std::is_same_v<f32, T>)
        {
            glTextureParameterf(textureId, param, value);
        }
        else if constexpr (std::is_integral_v<T>)
        {
            glTextureParameteri(textureId, param, value);
        }
        else if constexpr (std::is_same_v<T, Vector2>)
        {
            glTextureParameterfv(textureId, param, &value.x);
        }
        else if constexpr (std::is_same_v<T, Vector3>)
        {
            glTextureParameterfv(textureId, param, &value.x);
        }
        else if constexpr (std::is_same_v<T, Vector4>)
        {
            glTextureParameterfv(textureId, param, &value.x);
        }
        else
        {
            LOG_ERROR("Uniform type not supported!");
        }
    }

    // Framebuffer
    u32 createFramebuffer(u32 width, u32 height);
    u32 createFramebufferAttachment(u32 fbo, GPUFramebufferAttachmentType attachmentType, u32 width, u32 height);
    void setFramebufferAttachment(u32 fbo, u32 textureId, GPUFramebufferAttachmentType attachmentType);
    void enableFramebuffer(GPUFramebufferOperationType op, u32 FBO);
    void disableFramebuffer(GPUFramebufferOperationType op);
    void setFramebufferAttachmentToRead(GPUFramebufferAttachmentType attachmentType);
    Vector4 readFramebufferPixel(u32 x, u32 y, GPUTexturePixelFormat format);

    // Draw Call
    void drawElements(GPUDrawPrimitive drawPrimitive, u32 indicesCount, u32 instancesCount, bool instanced);

    // Clear
    void setClearColor(const Vector3& color);
    void clearColor();
    void clearDepth();
    void clearStencil();
    void clear();

    // Viewport
    void setViewport(u32 x, u32 y, u32 width, u32 height);

    // Flags
    void enableFlag(u32 flag);
    void disableFlag(u32 flag);

    // Blend
    void setBlendFunc(u32 sfactor, u32 dfactor);

    // Depth
    void setDepthFunc(u32 depthFunc);

    // Face mode
    void setFaceMode(GPUCullFaceType cullFaceType);

    // Shaders
    void enableProgram(u32 programId);
    void disableProgram(u32 programId);
    u32 compileProgram(const std::string& vertexShaderString, const std::string& fragmentShaderString);
    void bindSharedBufferToShader(u32 programId, GPUBufferType bufferType, const HashedString& bufferName, u32 bindingPoint);
    
    template<class T>
    void bindUniformValue(u32 programId, const HashedString& name, const T& value)
    {
        u32 location = glGetUniformLocation(programId, name.get().c_str());
        if constexpr (std::is_same_v<f32, T>)
        {
            glUniform1f(location, value);
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            glUniform1d(location, value);
        }
        else if constexpr (std::is_integral_v<T>)
        {
            if constexpr (std::is_unsigned_v<T>)
            {
                glUniform1ui(location, value);
            }
            else
            {
                glUniform1i(location, value);
            }
        }
        else if constexpr (std::is_same_v<T, Vector2>)
        {
            glUniform2fv(location, 1, &value.x);
        }
        else if constexpr (std::is_same_v<T, Vector3>)
        {
            glUniform3fv(location, 1, &value.x);
        }
        else if constexpr (std::is_same_v<T, Vector4>)
        {
            glUniform4fv(location, 1, &value.x);
        }
        else if constexpr (std::is_same_v<T, Matrix4>)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, value.getData());
        }
        else
        {
            LOG_ERROR("Uniform type not supported!");
        }
    }

    // Check Errors
    void setupGPUErrorHandling();
    void checkGPUErrors();
    void checkFramebufferErrors();
    bool checkShaderErrors(u32 shaderId, u32 statusToCheck, const std::string& tag, const std::string& logIfError);
    bool checkProgramErrors(u32 programId, u32 statusToCheck, const std::string& tag, const std::string& logIfError);
    static void gpuErrorMessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

private:
    void setBufferDataRaw(GPUBufferType bufferType, u32 VBO, u32 typeSize, u32 size, const void* data);
};
REGISTER_CLASS(GPUInterface)