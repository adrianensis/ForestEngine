#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"

#include <stddef.h>

GLuint GPUInterface::createBuffer()
{
	u32 bufferId = 0;
	glGenBuffers(1, &bufferId);
    bindBuffer(GPUBufferType::VERTEX, bufferId);
	return bufferId;
}

void GPUInterface::bindBuffer(GPUBufferType bufferType, u32 bufferId)
{
    glBindBuffer(static_cast<u32>(bufferType), bufferId);
}

void GPUInterface::attribute(u32 propertyArrayIndex, u32 elementSize, u32 primitiveType, u32 strideSize, u32 pointerOffset, u32 divisor)
{
	enableAttribute(propertyArrayIndex);
	if(primitiveType == GL_INT)
	{
		glVertexAttribIPointer(propertyArrayIndex, elementSize, primitiveType, strideSize, reinterpret_cast<byte*>(pointerOffset));
	}
	else
	{
		glVertexAttribPointer(propertyArrayIndex, elementSize, primitiveType, GL_FALSE, strideSize, reinterpret_cast<byte*>(pointerOffset));
	}

    glVertexAttribDivisor(propertyArrayIndex, divisor);
}

GLuint GPUInterface::createVertexBufferLayout()
{
	u32 vertexBufferLayout = 0;
	glGenVertexArrays(1, &vertexBufferLayout);
	enableVertexBufferLayout(vertexBufferLayout);
	return vertexBufferLayout;
}

u32 GPUInterface::getMaxElementsInSharedBuffer(GPUBufferType bufferType, u32 elementSizeInBytes)
{
    i32 maxElements = (getMaxBytesInSharedBuffer(bufferType)/elementSizeInBytes);
    return maxElements;
}

u32 GPUInterface::getMaxBytesInSharedBuffer(GPUBufferType bufferType)
{
    i32 maxBytes = 0;
    switch (bufferType)
    {
    case GPUBufferType::UNIFORM:
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxBytes);
        break;
    case GPUBufferType::STORAGE:
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxBytes);
        break;
    default:
        CHECK_MSG(false, "Ilegal GPUBufferType!");
        break;
    }

    return maxBytes;
}

u32 GPUInterface::getMaxBindingPointsForSharedBuffer(GPUBufferType bufferType)
{
    i32 maxBindingPoints = 0;
    switch (bufferType)
    {
    case GPUBufferType::UNIFORM:
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindingPoints);
        break;
    case GPUBufferType::STORAGE:
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxBindingPoints);
        break;
    default:
        CHECK_MSG(false, "Ilegal GPUBufferType!");
        break;
    }

    return maxBindingPoints;
}

void GPUInterface::bindSharedBufferToBindingPoint(GPUBufferType bufferType, u32 bufferId, u32 bindingPoint)
{
    // define the range of the buffer that links to a uniform binding point
    glBindBufferBase(static_cast<u32>(bufferType), bindingPoint, bufferId);
}

void GPUInterface::resizeBuffer(GPUBufferType bufferType, u32 bufferId, u32 typeSizeInBytes, u32 size, bool isStatic)
{
	bindBuffer(bufferType, bufferId);
    u32 usageHint = bufferType == GPUBufferType::STORAGE ? (isStatic ? GL_STATIC_COPY : GL_DYNAMIC_COPY) : (isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	glBufferData(static_cast<u32>(bufferType), typeSizeInBytes * size, nullptr, usageHint);
}

void GPUInterface::setBufferDataRaw(GPUBufferType bufferType, u32 bufferId, u32 typeSize, u32 size, const void* data)
{
	bindBuffer(bufferType, bufferId);
	glBufferSubData(static_cast<u32>(bufferType), 0, typeSize * size, data);
}

void GPUInterface::deleteVertexBufferLayout(u32 vertexBufferLayout)
{
    glDeleteVertexArrays(1, &vertexBufferLayout);
}

void GPUInterface::deleteBuffer(u32 bufferId)
{
    glDeleteBuffers(1, &bufferId);
}

void GPUInterface::enableAttribute(u32 attributeIndex)
{
	glEnableVertexAttribArray(attributeIndex);
}

void GPUInterface::disableAttribute(u32 attributeIndex)
{
	glDisableVertexAttribArray(attributeIndex);
}

void GPUInterface::enableVertexBufferLayout(u32 vertexBufferLayout)
{
	glBindVertexArray(vertexBufferLayout);
}

void GPUInterface::enableStencil(u32 stencilValue, u32 stencilFunction, u32 stencilPassOp)
{
    glEnable(GL_STENCIL_TEST);

    // if(isMask)
    // {
    //     // NOTE: Required stencilValue starts at 1!
    //     glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    //     glStencilFunc(GL_EQUAL, stencilValue-1, 0xFF);

    //     // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    //     // glDepthMask(GL_FALSE);
    // }
    // else
    // {
    //     glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    //     // Make it so the stencil test only passes when not equal to ref value
    //     glStencilFunc(stencilFunction, stencilValue-1, 0xFF);
    // }

    glStencilOp(GL_KEEP, GL_KEEP, stencilPassOp);
    glStencilFunc(stencilFunction, stencilValue, 0xFF);

}

void GPUInterface::disableStencil()
{
	// glStencilFunc(GL_ALWAYS, 0, 0xFF);
	// glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	// glDepthMask(GL_TRUE);

	glDisable(GL_STENCIL_TEST);
}

GLuint GPUInterface::createTexture(u32 width, u32 height, u32 format, const byte* data, bool createMipMap)
{
    u32 textureId = 0;
    glGenTextures(1, &textureId);

    enableTexture(textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(createMipMap)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    if(createMipMap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    disableTexture();

    return textureId;
}

GLuint GPUInterface::createTextureFont(u32 width, u32 height, u32 format, const byte* data)
{
    u32 textureId = 0;
    glGenTextures(1, &textureId);

    enableTexture(textureId);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    disableTexture();

    return textureId;
}

void GPUInterface::subTexture(u32 x, u32 y, u32 width, u32 height, u32 format, const byte* data)
{
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y , width, height, format, GL_UNSIGNED_BYTE, data);
}

void GPUInterface::deleteTexture(u32 textureId)
{
	glDeleteTextures(1, &textureId);
}

void GPUInterface::enableTexture(u32 textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void GPUInterface::disableTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GPUInterface::drawElements(u32 elementType, u32 indicesCount, u32 instancesCount, bool instanced)
{
	if(instanced)
	{
		glDrawElementsInstanced(elementType, indicesCount, GL_UNSIGNED_INT, 0, instancesCount);
	}
	else
	{
		glDrawElements(elementType, instancesCount * indicesCount, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
}

void GPUInterface::setClearColor(const Vector3& color)
{
    glClearColor(color.x, color.y, color.z ,1);
}

void GPUInterface::clearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void GPUInterface::clearStencil()
{
	glClear(GL_STENCIL_BUFFER_BIT);
}

void GPUInterface::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GPUInterface::setViewport(u32 x, u32 y, u32 width, u32 height)
{
    glViewport(x, y, width, height);
}

void GPUInterface::enableFlag(u32 flag)
{
    glEnable(flag);
}

void GPUInterface::disableFlag(u32 flag)
{
    glDisable(flag);
}

void GPUInterface::setBlendFunc(u32 sfactor, u32 dfactor)
{
    glBlendFunc(sfactor, dfactor);
}

void GPUInterface::setDepthFunc(u32 depthFunc)
{
    glDepthFunc(depthFunc);
}

void GPUInterface::setFaceMode(bool enableCull, u32 cullMode, u32 frontFaceOrientation)
{
    if(enableCull)
    {
        enableFlag(GL_CULL_FACE);	 // BACK by default
    }
    else
    {
        disableFlag(GL_CULL_FACE);
    }

    glCullFace(cullMode);
    glFrontFace(frontFaceOrientation);
}

void GPUInterface::enableProgram(u32 programId)
{
    glUseProgram(programId);
}

void GPUInterface::disableProgram(u32 programId)
{
    glUseProgram(0);
}

u32 GPUInterface::compileProgram(const std::string& vertexShaderString, const std::string& fragmentShaderString)
{

	const char* vertexCString = vertexShaderString.c_str();
	const char* fragmentCString = fragmentShaderString.c_str();

    u32 vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexCString, nullptr);
	glCompileShader(vertexShaderId);
    checkShaderErrors(vertexShaderId, GL_COMPILE_STATUS, "VERTEX SHADER", vertexShaderString);

	u32 fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentCString, nullptr);
	glCompileShader(fragmentShaderId);
    checkShaderErrors(fragmentShaderId, GL_COMPILE_STATUS, "FRAGMENT SHADER", fragmentShaderString);

	u32 programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
    checkProgramErrors(programId, GL_LINK_STATUS, "PROGRAM LINK", "-");

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

    return programId;
}

void GPUInterface::bindSharedBufferToShader(u32 programId, GPUBufferType bufferType, const std::string& bufferName, u32 bindingPoint)
{
    switch (bufferType)
    {
        case GPUBufferType::UNIFORM:
        {
            u32 location = glGetUniformBlockIndex(programId, bufferName.c_str());   
            glUniformBlockBinding(programId, location, bindingPoint);
            break;
        }
        case GPUBufferType::STORAGE:
        {
            u32 location = glGetProgramResourceIndex(programId, GL_SHADER_STORAGE_BLOCK, bufferName.c_str());
            glShaderStorageBlockBinding(programId, location, bindingPoint);
            break;
        }
        default:
            CHECK_MSG(false, "Not supported Buffer Type!");
        break;
    }
}

void GPUInterface::setupGPUErrorHandling()
{
    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        // initialize debug output 
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
        glDebugMessageCallback(gpuErrorMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
}

#define GPU_LOG_CASE(enumError, errorMessageOut) case enumError: errorMessageOut = #enumError; break;
#define GPU_LOG_CASE_DEFAULT(Tag, errorMessageOut) default : errorMessageOut = "Unknown " Tag " error"; break;

void GPUInterface::checkGPUErrors()
{
    i32 errorCode = glGetError();
	bool errorDetected = false;
    std::string errorMessage;

    const u32 MAX_GPU_LOGS = 50;
    u32 errorsIndex = 0;
	while(errorCode != GL_NO_ERROR)
	{
		errorDetected = true;
		switch(errorCode)
		{
			GPU_LOG_CASE(GL_INVALID_ENUM, errorMessage)
			GPU_LOG_CASE(GL_INVALID_VALUE, errorMessage)
			GPU_LOG_CASE(GL_INVALID_OPERATION, errorMessage)
			GPU_LOG_CASE(GL_OUT_OF_MEMORY, errorMessage)
			GPU_LOG_CASE(GL_STACK_OVERFLOW, errorMessage)
			GPU_LOG_CASE(GL_STACK_UNDERFLOW, errorMessage)
			GPU_LOG_CASE_DEFAULT("GL_ERROR", errorMessage)
		}
		LOG_ERROR(errorMessage);
		errorCode = glGetError();

        errorsIndex++;
        if(errorsIndex == MAX_GPU_LOGS)
        {
            LOG_ERROR("MAX GPU ERRORS REACHED");
            break;
        }
	}

    CHECK_MSG(!errorDetected, "GPU ERRORS DETECTED!");
}

void GPUInterface::checkFramebufferErrors()
{
    i32 errorCode = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    // checkGPUErrors();
	bool errorDetected = false;
    std::string errorMessage;

    if(errorCode != GL_FRAMEBUFFER_COMPLETE)
    {
        errorDetected = true;
        switch(errorCode)
        {
            GPU_LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT, errorMessage)
            GPU_LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, errorMessage)
            GPU_LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER, errorMessage)
            GPU_LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER, errorMessage)
            GPU_LOG_CASE(GL_FRAMEBUFFER_UNSUPPORTED, errorMessage)
            GPU_LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE, errorMessage)
            GPU_LOG_CASE(GL_FRAMEBUFFER_UNDEFINED, errorMessage)
            GPU_LOG_CASE_DEFAULT("GL_FRAMEBUFFER", errorMessage)
        }
    }

    CHECK_MSG(!errorDetected, "GPU ERRORS DETECTED!");
}

void GPUInterface::checkShaderErrors(u32 shaderId, u32 statusToCheck, const std::string& tag, const std::string& logIfError)
{
    i32 success = 0;
    glGetShaderiv(shaderId, statusToCheck, &success);
	if (!success)
	{
        i32 logSize = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize );
		if(logSize > 0)
        {
	        char* infoLog = new char[logSize];
            glGetShaderInfoLog(shaderId, logSize, nullptr, infoLog);
            LOG_ERROR(tag);
            LOG_ERROR(infoLog);
            delete[] infoLog;
            LOG_ERROR(logIfError);
        }
	}

    CHECK_MSG(success, "SHADER ERRORS DETECTED!");
}

void GPUInterface::checkProgramErrors(u32 programId, u32 statusToCheck, const std::string& tag, const std::string& logIfError)
{
    i32 success = 0;
    glGetProgramiv(programId, statusToCheck, &success);
	if (!success)
	{
        i32 logSize = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize );
		if(logSize > 0)
        {
	        char* infoLog = new char[logSize];
            glGetProgramInfoLog(programId, logSize, nullptr, infoLog);
            LOG_ERROR(tag);
            LOG_ERROR(infoLog);
            delete[] infoLog;
            LOG_ERROR(logIfError);
        }
	}

    CHECK_MSG(success, "SHADER ERRORS DETECTED!");
}

void GPUInterface::gpuErrorMessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::string logMessage;

    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    switch (source)
    {
        GPU_LOG_CASE(GL_DEBUG_SOURCE_API,logMessage)
        GPU_LOG_CASE(GL_DEBUG_SOURCE_WINDOW_SYSTEM,logMessage)
        GPU_LOG_CASE(GL_DEBUG_SOURCE_SHADER_COMPILER,logMessage)
        GPU_LOG_CASE(GL_DEBUG_SOURCE_THIRD_PARTY,logMessage)
        GPU_LOG_CASE(GL_DEBUG_SOURCE_APPLICATION,logMessage)
        GPU_LOG_CASE(GL_DEBUG_SOURCE_OTHER,logMessage)
    }
    LOG_TAG("GPU DEBUG", logMessage);

    switch (type)
    {
        GPU_LOG_CASE(GL_DEBUG_TYPE_ERROR,logMessage)
        GPU_LOG_CASE(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,logMessage)
        GPU_LOG_CASE(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,logMessage)
        GPU_LOG_CASE(GL_DEBUG_TYPE_PORTABILITY,logMessage)
        GPU_LOG_CASE(GL_DEBUG_TYPE_PERFORMANCE,logMessage)
        GPU_LOG_CASE(GL_DEBUG_TYPE_MARKER,logMessage)
        GPU_LOG_CASE(GL_DEBUG_TYPE_PUSH_GROUP,logMessage)
        GPU_LOG_CASE(GL_DEBUG_TYPE_POP_GROUP,logMessage)
        GPU_LOG_CASE(GL_DEBUG_TYPE_OTHER,logMessage)
    }
    LOG_TAG("GPU DEBUG", logMessage);
    
    switch (severity)
    {
        GPU_LOG_CASE(GL_DEBUG_SEVERITY_HIGH,logMessage)
        GPU_LOG_CASE(GL_DEBUG_SEVERITY_MEDIUM,logMessage)
        GPU_LOG_CASE(GL_DEBUG_SEVERITY_LOW,logMessage)
        GPU_LOG_CASE(GL_DEBUG_SEVERITY_NOTIFICATION,logMessage)
    }
    LOG_TAG("GPU DEBUG", logMessage);
    LOG_TAG("GPU DEBUG", std::to_string(id));
    LOG_TAG("GPU DEBUG", message);

    CHECK_MSG(type != GL_DEBUG_TYPE_ERROR, "GPU ERROR DETECTED!");
}