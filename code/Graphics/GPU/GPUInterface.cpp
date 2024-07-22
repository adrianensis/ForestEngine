#include "Graphics/GPU/GPUInterface.hpp"
#include <stddef.h>

bool GPUInterface::loadAPI()
{
    PROFILER_CPU()
    bool result = true;
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        CHECK_MSG(false, "GLEW failed.");
        result = false;
    }
    return result;
}

u32 GPUInterface::createBuffer(GPUBufferType bufferType)
{
    PROFILER_CPU()
    PROFILER_GPU()
	u32 bufferId = 0;
	glCreateBuffers(1, &bufferId);
    bindBuffer(bufferType, bufferId);
	return bufferId;
}

void GPUInterface::bindBuffer(GPUBufferType bufferType, u32 bufferId)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glBindBuffer(TO_U32(bufferType), bufferId);
}

void GPUInterface::attribute(u32 propertyArrayIndex, u32 elementSize, GPUPrimitiveDataType primitiveType, u32 strideSize, u32 pointerOffset, u32 divisor)
{
    PROFILER_CPU()
    PROFILER_GPU()
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
    PROFILER_CPU()
    PROFILER_GPU()
	u32 vertexBufferLayout = 0;
	glGenVertexArrays(1, &vertexBufferLayout);
	enableVertexBufferLayout(vertexBufferLayout);
	return vertexBufferLayout;
}

u32 GPUInterface::getMaxElementsInSharedBuffer(GPUBufferType bufferType, u32 elementSizeInBytes)
{
    PROFILER_CPU()
    PROFILER_GPU()
    i32 maxElements = (getMaxBytesInSharedBuffer(bufferType)/elementSizeInBytes);
    return maxElements;
}

u32 GPUInterface::getMaxBytesInSharedBuffer(GPUBufferType bufferType)
{
    PROFILER_CPU()
    PROFILER_GPU()
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
    PROFILER_CPU()
    PROFILER_GPU()
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
    PROFILER_CPU()
    PROFILER_GPU()
    // define the range of the buffer that links to a uniform binding point
    glBindBufferBase(TO_U32(bufferType), bindingPoint, bufferId);
}

void GPUInterface::resizeBuffer(GPUBufferType bufferType, u32 bufferId, u32 typeSizeInBytes, u32 size, bool isStatic)
{
    PROFILER_CPU()
    PROFILER_GPU()
	// bindBuffer(bufferType, bufferId);
    u32 usageHint = (isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	glNamedBufferData(bufferId, typeSizeInBytes * size, nullptr, usageHint);
}

void GPUInterface::setBufferDataRaw(GPUBufferType bufferType, u32 bufferId, u32 typeSize, u32 size, const void* data)
{
    PROFILER_CPU()
    PROFILER_GPU()
	// bindBuffer(bufferType, bufferId);
	glNamedBufferSubData(bufferId, 0, typeSize * size, data);
}

void GPUInterface::deleteVertexBufferLayout(u32 vertexBufferLayout)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glDeleteVertexArrays(1, &vertexBufferLayout);
}

void GPUInterface::deleteBuffer(u32 bufferId)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glDeleteBuffers(1, &bufferId);
}

void GPUInterface::enableAttribute(u32 attributeIndex)
{
    PROFILER_CPU()
    PROFILER_GPU()
	glEnableVertexAttribArray(attributeIndex);
}

void GPUInterface::disableAttribute(u32 attributeIndex)
{
    PROFILER_CPU()
    PROFILER_GPU()
	glDisableVertexAttribArray(attributeIndex);
}

void GPUInterface::enableVertexBufferLayout(u32 vertexBufferLayout)
{
    PROFILER_CPU()
    PROFILER_GPU()
	glBindVertexArray(vertexBufferLayout);
}

void GPUInterface::enableStencil(u32 stencilValue, GPUStencilFunction stencilFunction, GPUStencilOp stencilPassOp)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, TO_U32(stencilPassOp));
    glStencilFunc(TO_U32(stencilFunction), stencilValue, 0xFF);
}

void GPUInterface::disableStencil()
{
    PROFILER_CPU()
    PROFILER_GPU()
	glDisable(GL_STENCIL_TEST);
}

u32 GPUInterface::createTexture(GPUTextureFormat internalformat, u32 width, u32 height, bool createMipMap)
{
    PROFILER_CPU()
    PROFILER_GPU()
    u32 textureId = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureId);

    setPixelStoreMode(GL_UNPACK_ALIGNMENT, 4);

    setTextureParameter(textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    setTextureParameter(textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(createMipMap)
    {
        setTextureParameter(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        setTextureParameter(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        setTextureParameter(textureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        setTextureParameter(textureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    u32 maxLevels = 6;

    setTextureParameter(textureId, GL_TEXTURE_BASE_LEVEL, 0);
    setTextureParameter(textureId, GL_TEXTURE_MAX_LEVEL, maxLevels);
    // setTextureParameter(textureId, GL_TEXTURE_LOD_BIAS, 1.5f);
    
    f32 maxAnisotropy = 0;
    getValue<f32>(GL_MAX_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);
    setTextureParameter(textureId, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);

    u32 levels = 1 + std::floor(std::log2(std::max(width, height)));
    levels = std::min(maxLevels, levels);
    setTextureStorage(textureId, levels, internalformat, width, height);

    return textureId;
}

u32 GPUInterface::createTexture1ByteChannel(u32 width, u32 height, const byte* data)
{
    PROFILER_CPU()
    PROFILER_GPU()
    u32 textureId = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureId);

    // disable byte-alignment restriction
    setPixelStoreMode(GL_UNPACK_ALIGNMENT, 1);
    setTextureParameter(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    setTextureParameter(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    setTextureParameter(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    setTextureParameter(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    setTextureStorage(textureId, 1, GPUTextureFormat::R8, width, height);

    return textureId;
}

void GPUInterface::setTextureData(u32 textureId, u32 width, u32 height, GPUTexturePixelFormat format, GPUPrimitiveDataType type, bool createMipMap, const byte* data)
{
    PROFILER_CPU()
    PROFILER_GPU()
    setSubTexture(textureId, 0, 0, width, height, format, type, data);
    if(createMipMap)
    {
        glGenerateTextureMipmap(textureId);
    }
}

void GPUInterface::setTextureStorage(u32 textureId, u32 levels, GPUTextureFormat internalformat, u32 width, u32 height)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glTextureStorage2D(textureId, levels, TO_U32(internalformat), width, height);
}

void GPUInterface::setSubTexture(u32 textureId, u32 x, u32 y, u32 width, u32 height, GPUTexturePixelFormat format, GPUPrimitiveDataType type, const byte* data)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glTextureSubImage2D(textureId, 0, x, y , width, height, TO_U32(format), TO_U32(type), data);
}

void GPUInterface::deleteTexture(u32 textureId)
{
    PROFILER_CPU()
    PROFILER_GPU()
	glDeleteTextures(1, &textureId);
}

void GPUInterface::enableTexture(u32 textureId, u32 textureUnit, GPUPipelineStage stage)
{
    PROFILER_CPU()
    PROFILER_GPU()
    u32 maxTextureUntis = getMaxTextureUnits(stage);
    CHECK_MSG(textureUnit < maxTextureUntis, "Max Texture Unit reached!");

    glBindTextureUnit(textureUnit, textureId);
}

void GPUInterface::disableTexture(u32 textureUnit, GPUPipelineStage stage)
{
    PROFILER_CPU()
    PROFILER_GPU()
    u32 maxTextureUntis = getMaxTextureUnits(stage);
    CHECK_MSG(textureUnit < maxTextureUntis, "Max Texture Unit reached!");
    glBindTextureUnit(textureUnit, 0);
}

void GPUInterface::setPixelStoreMode(u32 param, u32 value)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glPixelStorei(param, value);
}

u32 GPUInterface::getMaxTextureUnits(GPUPipelineStage stage)
{
    PROFILER_CPU()
    PROFILER_GPU()
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

TextureHandle GPUInterface::getTextureHandle(u32 texture​Id)
{
    PROFILER_CPU()
    PROFILER_GPU()
    return glGetTextureHandleARB(texture​Id);
}

void GPUInterface::makeTextureResident(TextureHandle handle​, bool makeResident)
{
    PROFILER_CPU()
    PROFILER_GPU()
    if(makeResident)
    {
        glMakeTextureHandleResidentARB(handle​);
    }
    else
    {
        glMakeTextureHandleNonResidentARB(handle​);
    }
}

u32 GPUInterface::createFramebuffer(u32 width, u32 height)
{
    PROFILER_CPU()
    PROFILER_GPU()
    u32 FBO = 0;
    // Create the FBO
    glGenFramebuffers(1, &FBO);
    enableFramebuffer(GPUFramebufferOperationType::READ_AND_DRAW, FBO);

    // Restore the default framebuffer
    // disableTexture();
    disableFramebuffer(GPUFramebufferOperationType::READ_AND_DRAW);

    return FBO;
}

u32 GPUInterface::createFramebufferAttachment(u32 fbo, GPUFramebufferAttachmentType attachmentType, u32 width, u32 height)
{
    PROFILER_CPU()
    PROFILER_GPU()
    u32 textureId = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureId);

    setTextureParameter(textureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    setTextureParameter(textureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    setTextureParameter(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    setTextureParameter(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    setTextureParameter(textureId, GL_TEXTURE_BORDER_COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    CHECK_MSG(attachmentType > GPUFramebufferAttachmentType::NONE, "NONE is not valid attachment!");

    if (attachmentType >= GPUFramebufferAttachmentType::COLOR0 && attachmentType <= GPUFramebufferAttachmentType::COLOR15)
    {
        setTextureStorage(textureId, 1, GPUTextureFormat::RGBA8, width, height);
        // setTextureData(textureId, width, height, GPUTexturePixelFormat::RGBA, GPUPrimitiveDataType::UNSIGNED_BYTE, 0);
    } 
    else if (attachmentType == GPUFramebufferAttachmentType::DEPTH)
    {
        setTextureStorage(textureId, 1, GPUTextureFormat::DEPTH_COMPONENT32F, width, height);
        // setTextureData(textureId, width, height, GPUTexturePixelFormat::DEPTH_COMPONENT, GPUPrimitiveDataType::UNSIGNED_BYTE, 0);
    } 
    else if (attachmentType == GPUFramebufferAttachmentType::STENCIL)
    {
        setTextureStorage(textureId, 1, GPUTextureFormat::STENCIL_INDEX8, width, height);
        // setTextureData(textureId, width, height, GPUTexturePixelFormat::STENCIL_INDEX, GPUPrimitiveDataType::UNSIGNED_BYTE, 0);
    }
    else if (attachmentType == GPUFramebufferAttachmentType::DEPTH_STENCIL)
    {
        setTextureStorage(textureId, 1, GPUTextureFormat::DEPTH24_STENCIL8, width, height);
        // setTextureData(textureId, width, height, GPUTexturePixelFormat::DEPTH_STENCIL, GPUPrimitiveDataType::UNSIGNED_BYTE, 0);
    }

    setFramebufferAttachment(fbo, textureId, attachmentType);

    // Verify that the FBO is correct
    checkFramebufferErrors();

    return textureId;
}

void GPUInterface::setFramebufferAttachment(u32 fbo, u32 textureId, GPUFramebufferAttachmentType attachmentType)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glNamedFramebufferTexture(fbo, TO_U32(attachmentType), textureId, 0);
}

void GPUInterface::enableFramebuffer(GPUFramebufferOperationType op, u32 FBO)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glBindFramebuffer(TO_U32(op), FBO);
}

void GPUInterface::disableFramebuffer(GPUFramebufferOperationType op)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glBindFramebuffer(TO_U32(op), 0);
}

void GPUInterface::setFramebufferAttachmentToRead(GPUFramebufferAttachmentType attachmentType)
{
    PROFILER_CPU()
    PROFILER_GPU()
    CHECK_MSG(attachmentType == GPUFramebufferAttachmentType::NONE ||
        (attachmentType >= GPUFramebufferAttachmentType::COLOR0 &&
        attachmentType <= GPUFramebufferAttachmentType::COLOR15), "Only COLOR or NONE attachment is suitable for reading!");
    glReadBuffer(TO_U32(attachmentType));
}

Vector4 GPUInterface::readFramebufferPixel(u32 x, u32 y, GPUTexturePixelFormat format)
{
    PROFILER_CPU()
    PROFILER_GPU()
    Vector4 pixelColor;
    glReadPixels(x, y, 1, 1, TO_U32(format), TO_U32(GPUPrimitiveDataType::FLOAT), &pixelColor);
    return pixelColor;
}

void GPUInterface::drawElements(GPUDrawPrimitive drawPrimitive, u32 indicesCount, u32 instancesCount, bool instanced)
{
    PROFILER_CPU()
    PROFILER_GPU()
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
    PROFILER_CPU()
    PROFILER_GPU()
    glClearColor(color.x, color.y, color.z ,1);
}

void GPUInterface::clearColor()
{
    PROFILER_CPU()
    PROFILER_GPU()
	glClear(GL_COLOR_BUFFER_BIT);
}

void GPUInterface::clearDepth()
{
    PROFILER_CPU()
    PROFILER_GPU()
	glClear(GL_DEPTH_BUFFER_BIT);
}

void GPUInterface::clearStencil()
{
    PROFILER_CPU()
    PROFILER_GPU()
	glClear(GL_STENCIL_BUFFER_BIT);
}

void GPUInterface::clear()
{
    PROFILER_CPU()
    PROFILER_GPU()
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GPUInterface::setViewport(u32 x, u32 y, u32 width, u32 height)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glViewport(x, y, width, height);
}

void GPUInterface::enableFlag(GPUFlags flag)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glEnable(TO_U32(flag));
}

void GPUInterface::disableFlag(GPUFlags flag)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glDisable(TO_U32(flag));
}

void GPUInterface::setBlendFunc(GPUBlendFactor sfactor, GPUBlendFactor dfactor)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glBlendFunc(TO_U32(sfactor), TO_U32(dfactor));
}

void GPUInterface::setDepthFunc(GPUDepthFunc depthFunc)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glDepthFunc(TO_U32(depthFunc));
}

void GPUInterface::setFaceMode(GPUCullFaceType cullFaceType)
{
    PROFILER_CPU()
    PROFILER_GPU()
    bool enable = cullFaceType != GPUCullFaceType::NONE;
    if(enable)
    {
        enableFlag(GPUFlags::CULL_FACE);	 // BACK by default
        glCullFace(TO_U32(cullFaceType));
        // glFrontFace(GL_CCW); // GL_CCW by default
    }
    else
    {
        disableFlag(GPUFlags::CULL_FACE);
    }
}

void GPUInterface::enableProgram(u32 programId)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glUseProgram(programId);
}

void GPUInterface::disableProgram(u32 programId)
{
    PROFILER_CPU()
    PROFILER_GPU()
    glUseProgram(0);
}

u32 GPUInterface::compileProgram(const std::string& vertexShaderString, const std::string& fragmentShaderString)
{
    PROFILER_CPU()
    PROFILER_GPU()

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
    PROFILER_CPU()
    PROFILER_GPU()
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