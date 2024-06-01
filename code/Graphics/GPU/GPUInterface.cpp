#include "Graphics/GPU/GPUInterface.hpp"
#include <stddef.h>

u32 GPUInterface::createBuffer()
{
	u32 bufferId = 0;
	glGenBuffers(1, &bufferId);
	return bufferId;
}

void GPUInterface::bindBuffer(GPUBufferType bufferType, u32 bufferId)
{
    glBindBuffer(TO_U32(bufferType), bufferId);
}

void GPUInterface::attribute(u32 propertyArrayIndex, u32 elementSize, GPUPrimitiveDataType primitiveType, u32 strideSize, u32 pointerOffset, u32 divisor)
{
	enableAttribute(propertyArrayIndex);
	if(primitiveType == GPUPrimitiveDataType::INT || primitiveType == GPUPrimitiveDataType::UNSIGNED_INT)
	{
		glVertexAttribIPointer(propertyArrayIndex, elementSize, TO_U32(primitiveType), strideSize, reinterpret_cast<byte*>(pointerOffset));
	}
	else
	{
		glVertexAttribPointer(propertyArrayIndex, elementSize, TO_U32(primitiveType), GL_FALSE, strideSize, reinterpret_cast<byte*>(pointerOffset));
	}

    glVertexAttribDivisor(propertyArrayIndex, divisor);
}

u32 GPUInterface::createVertexBufferLayout()
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
    glBindBufferBase(TO_U32(bufferType), bindingPoint, bufferId);
}

void GPUInterface::resizeBuffer(GPUBufferType bufferType, u32 bufferId, u32 typeSizeInBytes, u32 size, bool isStatic)
{
	bindBuffer(bufferType, bufferId);
    u32 usageHint = bufferType == GPUBufferType::STORAGE ? (isStatic ? GL_STATIC_COPY : GL_DYNAMIC_COPY) : (isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	glBufferData(TO_U32(bufferType), typeSizeInBytes * size, nullptr, usageHint);
}

void GPUInterface::setBufferDataRaw(GPUBufferType bufferType, u32 bufferId, u32 typeSize, u32 size, const void* data)
{
	bindBuffer(bufferType, bufferId);
	glBufferSubData(TO_U32(bufferType), 0, typeSize * size, data);
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

u32 GPUInterface::createTexture(GPUTextureFormat internalformat, u32 width, u32 height, GPUTexturePixelFormat format, const byte* data, bool createMipMap)
{
    u32 textureId = 0;
    glGenTextures(1, &textureId);

    bindTexture(textureId);

    setTextureParameter<u32>(GL_TEXTURE_WRAP_S, GL_REPEAT);	
    setTextureParameter<u32>(GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(createMipMap)
    {
        setTextureParameter<u32>(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // setTextureParameter<u32>(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        setTextureParameter<u32>(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        setTextureParameter<u32>(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    
    setTextureFormatWithData(internalformat, width, height, format, GPUPrimitiveDataType::UNSIGNED_BYTE, data);
    
    if(createMipMap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    disableTexture();

    return textureId;
}

u32 GPUInterface::createTexture1ByteChannel(u32 width, u32 height, const byte* data)
{
    u32 textureId = 0;
    glGenTextures(1, &textureId);

    bindTexture(textureId);

    // disable byte-alignment restriction
    setPixelStoreMode(GL_UNPACK_ALIGNMENT, 1);
    setTextureFormatWithData(GPUTextureFormat::RED, width, height, GPUTexturePixelFormat::RED, GPUPrimitiveDataType::UNSIGNED_BYTE, data);

    setTextureParameter<u32>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    setTextureParameter<u32>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    setTextureParameter<u32>(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    setTextureParameter<u32>(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    disableTexture();

    return textureId;
}

void GPUInterface::subTexture(u32 x, u32 y, u32 width, u32 height, GPUTextureFormat format, const byte* data)
{
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y , width, height, TO_U32(format), TO_U32(GPUPrimitiveDataType::UNSIGNED_BYTE), data);
}

void GPUInterface::setTextureFormatWithData(GPUTextureFormat internalformat, u32 width, u32 height, GPUTexturePixelFormat format, GPUPrimitiveDataType type, const byte* data)
{
    glTexImage2D(GL_TEXTURE_2D, 0, TO_U32(internalformat), width, height, 0, TO_U32(format), TO_U32(type), data);
}

void GPUInterface::setTextureFormat(GPUTextureFormat internalformat, u32 width, u32 height, GPUTexturePixelFormat format, GPUPrimitiveDataType type)
{
    setTextureFormatWithData(internalformat, width, height, format, type, 0);
}

void GPUInterface::deleteTexture(u32 textureId)
{
	glDeleteTextures(1, &textureId);
}

void GPUInterface::enableTexture(u32 textureId, u32 textureUnit, GPUPipelineStage stage)
{
    u32 maxTextureUntis = getMaxTextureUnits(stage);
    CHECK_MSG(textureUnit < maxTextureUntis, "Max Texture Unit reached!");

    glActiveTexture(GL_TEXTURE0 + textureUnit);
	bindTexture(textureId);
}

void GPUInterface::bindTexture(u32 textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void GPUInterface::disableTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GPUInterface::setPixelStoreMode(u32 param, u32 value)
{
    glPixelStorei(param, value);
}

u32 GPUInterface::getMaxTextureUnits(GPUPipelineStage stage)
{
    i32 maxTextureImageUnits = 0;
    switch (stage)
    {
        case GPUPipelineStage::NONE: CHECK_MSG(false, "Invalid GPUPipelineStages!"); break;
        case GPUPipelineStage::VERTEX: getValue<i32>(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, maxTextureImageUnits); break;
        case GPUPipelineStage::FRAGMENT: getValue<i32>(GL_MAX_TEXTURE_IMAGE_UNITS, maxTextureImageUnits); break;
        case GPUPipelineStage::GEOMETRY: getValue<i32>(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, maxTextureImageUnits); break;
        case GPUPipelineStage::TESS_CONTROL: getValue<i32>(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, maxTextureImageUnits); break;
        case GPUPipelineStage::TESS_EVALUATION: getValue<i32>(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, maxTextureImageUnits); break;
        case GPUPipelineStage::COMPUTE: getValue<i32>(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, maxTextureImageUnits); break;
    }

    return maxTextureImageUnits;
}

u32 GPUInterface::createFramebuffer(u32 width, u32 height)
{
    u32 FBO = 0;
    // Create the FBO
    glGenFramebuffers(1, &FBO);
    enableFramebuffer(GPUFramebufferOperationType::READ_AND_DRAW, FBO);

    // Restore the default framebuffer
    // disableTexture();
    disableFramebuffer(GPUFramebufferOperationType::READ_AND_DRAW);

    return FBO;
}

u32 GPUInterface::createFramebufferAttachment(GPUFramebufferAttachmentType attachmentType, u32 width, u32 height)
{
    u32 mTextureId = 0;
    glGenTextures(1, &mTextureId);
    bindTexture(mTextureId);

    CHECK_MSG(attachmentType > GPUFramebufferAttachmentType::NONE, "NONE is not valid attachment!");

    if (attachmentType >= GPUFramebufferAttachmentType::COLOR0 && attachmentType <= GPUFramebufferAttachmentType::COLOR31)
    {
        setTextureFormat(GPUTextureFormat::RGBA, width, height, GPUTexturePixelFormat::RGBA, GPUPrimitiveDataType::FLOAT);
    } 
    else if (attachmentType == GPUFramebufferAttachmentType::DEPTH)
    {
        setTextureFormat(GPUTextureFormat::DEPTH_COMPONENT, width, height, GPUTexturePixelFormat::DEPTH_COMPONENT, GPUPrimitiveDataType::FLOAT);
    } 
    else if (attachmentType == GPUFramebufferAttachmentType::STENCIL)
    {
        setTextureFormat(GPUTextureFormat::STENCIL_INDEX, width, height, GPUTexturePixelFormat::STENCIL_INDEX, GPUPrimitiveDataType::UNSIGNED_BYTE);
    }
    else if (attachmentType == GPUFramebufferAttachmentType::DEPTH_STENCIL)
    {
        setTextureFormat(GPUTextureFormat::DEPTH_STENCIL, width, height, GPUTexturePixelFormat::DEPTH_STENCIL, GPUPrimitiveDataType::UNSIGNED_INT_24_8);
    }

    setTextureParameter<u32>(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    setTextureParameter<u32>(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    setTextureParameter<u32>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    setTextureParameter<u32>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    setTextureParameter<Vector4>(GL_TEXTURE_BORDER_COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    setFramebufferAttachment(mTextureId, attachmentType);

    // Verify that the FBO is correct
    // checkFramebufferErrors();

    return mTextureId;
}

void GPUInterface::setFramebufferAttachment(u32 textureId, GPUFramebufferAttachmentType attachmentType)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, TO_U32(attachmentType), GL_TEXTURE_2D, textureId, 0);
}

void GPUInterface::enableFramebuffer(GPUFramebufferOperationType op, u32 FBO)
{
    glBindFramebuffer(TO_U32(op), FBO);
}

void GPUInterface::disableFramebuffer(GPUFramebufferOperationType op)
{
    glBindFramebuffer(TO_U32(op), 0);
}

void GPUInterface::setFramebufferAttachmentToRead(GPUFramebufferAttachmentType attachmentType)
{
    CHECK_MSG(attachmentType == GPUFramebufferAttachmentType::NONE ||
        (attachmentType >= GPUFramebufferAttachmentType::COLOR0 &&
        attachmentType <= GPUFramebufferAttachmentType::COLOR31), "Only COLOR or NONE attachment is suitable for reading!");
    glReadBuffer(TO_U32(attachmentType));
}

Vector4 GPUInterface::readFramebufferPixel(u32 x, u32 y, GPUTexturePixelFormat format)
{
    Vector4 pixelColor;
    glReadPixels(x, y, 1, 1, TO_U32(format), TO_U32(GPUPrimitiveDataType::FLOAT), &pixelColor);
    return pixelColor;
}

void GPUInterface::drawElements(GPUDrawPrimitive drawPrimitive, u32 indicesCount, u32 instancesCount, bool instanced)
{
	if(instanced)
	{
		glDrawElementsInstanced(TO_U32(drawPrimitive), indicesCount, TO_U32(GPUPrimitiveDataType::UNSIGNED_INT), 0, instancesCount);
	}
	else
	{
		glDrawElements(TO_U32(drawPrimitive), instancesCount * indicesCount, TO_U32(GPUPrimitiveDataType::UNSIGNED_INT), 0);
	}

	glBindVertexArray(0);
}

void GPUInterface::setClearColor(const Vector3& color)
{
    glClearColor(color.x, color.y, color.z ,1);
}

void GPUInterface::clearColor()
{
	glClear(GL_COLOR_BUFFER_BIT);
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

void GPUInterface::setFaceMode(GPUCullFaceType cullFaceType)
{
    bool enable = cullFaceType != GPUCullFaceType::NONE;
    if(enable)
    {
        enableFlag(GL_CULL_FACE);	 // BACK by default
        glCullFace(static_cast<u32>(cullFaceType));
        // glFrontFace(GL_CCW); // GL_CCW by default
    }
    else
    {
        disableFlag(GL_CULL_FACE);
    }
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

    bool success = true;
    u32 vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexCString, nullptr);
	glCompileShader(vertexShaderId);
    success = success && checkShaderErrors(vertexShaderId, GL_COMPILE_STATUS, "VERTEX SHADER", vertexShaderString);

	u32 fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentCString, nullptr);
	glCompileShader(fragmentShaderId);
    success = success && checkShaderErrors(fragmentShaderId, GL_COMPILE_STATUS, "FRAGMENT SHADER", fragmentShaderString);

	u32 programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
    success = success && checkProgramErrors(programId, GL_LINK_STATUS, "PROGRAM LINK", "-");

    CHECK_MSG(success, "SHADER ERRORS DETECTED!");

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

    return programId;
}

void GPUInterface::bindSharedBufferToShader(u32 programId, GPUBufferType bufferType, const HashedString& bufferName, u32 bindingPoint)
{
    switch (bufferType)
    {
        case GPUBufferType::UNIFORM:
        {
            u32 location = glGetUniformBlockIndex(programId, bufferName.get().c_str());   
            glUniformBlockBinding(programId, location, bindingPoint);
            break;
        }
        case GPUBufferType::STORAGE:
        {
            u32 location = glGetProgramResourceIndex(programId, GL_SHADER_STORAGE_BLOCK, bufferName.get().c_str());
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
    int flags;
    getValue<i32>(GL_CONTEXT_FLAGS, flags);
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

bool GPUInterface::checkShaderErrors(u32 shaderId, u32 statusToCheck, const std::string& tag, const std::string& logIfError)
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

    // CHECK_MSG(success, "SHADER ERRORS DETECTED!");

    return success != 0;
}

bool GPUInterface::checkProgramErrors(u32 programId, u32 statusToCheck, const std::string& tag, const std::string& logIfError)
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

    // CHECK_MSG(success, "SHADER ERRORS DETECTED!");
    return success != 0;
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