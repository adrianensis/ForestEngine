#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"

#include <stddef.h>


Vector2 RenderContext::getWindowSize()
{
	return smWindowSize;
}

f32 RenderContext::getAspectRatio()
{
	return smWindowSize.x / smWindowSize.y;
}

void RenderContext::init()
{
	TRACE()

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE); // https://stackoverflow.com/questions/50412575/is-there-a-way-to-remove-60-fps-cap-in-glfw

	smWindowSize.set(1080, 720);

	smWindow = glfwCreateWindow(smWindowSize.x, smWindowSize.y, "Engine", NULL, NULL);

	if (smWindow)
	{
		glfwMakeContextCurrent(smWindow);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
		}

		glfwSwapInterval(0);

		glfwSetFramebufferSizeCallback(smWindow, onResize);

		glClearColor(0,0.3,0.3,1);
		glEnable(GL_DEPTH_TEST); // Enable depth testing
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);	 // BACK by default
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		clear();
	}
	else
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
}

bool RenderContext::isClosed()
{
	return glfwWindowShouldClose(smWindow);
}

void RenderContext::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderContext::swap()
{
	// https://www.khronos.org/opengl/wiki/Common_Mistakes
	// section: glFinish and glFlush
	glFlush();
	glfwSwapBuffers(smWindow);
}

void RenderContext::terminate()
{
	glfwDestroyWindow(smWindow);
	glfwTerminate();
}

GLuint RenderContext::createVBO()
{
	u32 VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	return VBO;
}

void RenderContext::attribute(u32 propertyArrayIndex, u32 elementSize, u32 primitiveType, u32 strideSize, u32 pointerOffset, u32 divisor)
{
	enableProperty(propertyArrayIndex);
	if(primitiveType == GL_INT)
	{
		glVertexAttribIPointer(propertyArrayIndex, elementSize, primitiveType, strideSize, (byte*) pointerOffset);
	}
	else
	{
		glVertexAttribPointer(propertyArrayIndex, elementSize, primitiveType, GL_FALSE, strideSize, (byte*) pointerOffset);
	}

	if(divisor > 0)
	{
		glVertexAttribDivisor(propertyArrayIndex, divisor);
	}
}

GLuint RenderContext::createVAO()
{
	u32 VAO;
	glGenVertexArrays(1, &VAO);
	enableVAO(VAO);
	return VAO;
}

GLuint RenderContext::createEBO()
{
	u32 EBO;
	glGenBuffers(1, &EBO);
	return EBO;
}

void RenderContext::resizeVBO(u32 VBO, u32 size, u32 drawMode /*= GL_DYNAMIC_DRAW*/)
{
	resizeVBOAnyType(VBO, sizeof(f32), size,drawMode);
}

void RenderContext::resizeVBOU32(u32 VBO, u32 size, u32 drawMode /*= GL_DYNAMIC_DRAW*/)
{
	resizeVBOAnyType(VBO, sizeof(u32), size,drawMode);
}

void RenderContext::resizeVBOAnyType(u32 VBO, u32 typeSizeInBytes, u32 size, u32 drawMode /*= GL_DYNAMIC_DRAW*/)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, typeSizeInBytes * size, nullptr, drawMode);
}

void RenderContext::resizeEBO(u32 EBO, u32 size, u32 drawMode /*= GL_DYNAMIC_DRAW*/)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * size, nullptr, drawMode);
}

void RenderContext::setDataVBO(u32 VBO, const std::vector<f32>& data)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(f32) * data.size(), data.data());
}

void RenderContext::setDataVBOU32(u32 VBO, const std::vector<u32>& data)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(u32) * data.size(), data.data());
}

void RenderContext::setDataVBOAnyTypeRaw(u32 VBO, u32 typeSize, u32 size, const void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, typeSize * size, data);
}

void RenderContext::setDataEBO(u32 EBO, const std::vector<Face>& data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(Face) * data.size(), data.data());
}

void RenderContext::setDataEBORaw(u32 EBO, const std::vector<u32>& data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(u32) * data.size(), data.data());
}

void RenderContext::deleteVAO(u32 VAO)
{
    glDeleteVertexArrays(1, &VAO);
}

void RenderContext::deleteVBO(u32 VBO)
{
    glDeleteBuffers(1, &VBO);
}

void RenderContext::deleteEBO(u32 EBO)
{
    glDeleteBuffers(1, &EBO);
}

void RenderContext::enableProperty(u32 propertyArrayIndex)
{
	glEnableVertexAttribArray(propertyArrayIndex);
}

void RenderContext::disableProperty(u32 propertyArrayIndex)
{
	glDisableVertexAttribArray(propertyArrayIndex);
}

void RenderContext::enableVAO(u32 VAO)
{
	glBindVertexArray(VAO);
}

void RenderContext::enableStencil(bool isMask, u32 stencilValue, u32 stencilFunction)
{
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    if(isMask)
    {
        // Make it so the stencil test always passes
        glStencilFunc(GL_ALWAYS, stencilValue, 0xFF);
        // Enable modifying of the stencil buffer
        glStencilMask(0xFF);

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
    }
    else
    {
        // Make it so the stencil test only passes when not equal to ref value
        glStencilFunc(stencilFunction, stencilValue, 0xFF);
        // Disable modifying of the stencil buffer
        glStencilMask(0x00);
    }
}

void RenderContext::disableStencil()
{
    glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	glDisable(GL_STENCIL_TEST);
}

GLuint RenderContext::createTexture(u32 width, u32 height, const byte* data)
{
    u32 textureId;
    glGenTextures(1, &textureId);

    enableTexture(textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_TEXTURE_MAG_FILTER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    disableTexture();

    return textureId;
}

void RenderContext::deleteTexture(u32 textureId)
{
	glDeleteTextures(1, &textureId);
}

void RenderContext::enableTexture(u32 textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void RenderContext::disableTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderContext::drawElements(u32 indicesCount, u32 instancesCount, bool instanced)
{
	if(instanced)
	{
		glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0, instancesCount);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, instancesCount * indicesCount, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
}

void RenderContext::drawLines(u32 linesCount)
{
	glDrawElements(GL_LINES, linesCount * 2, GL_UNSIGNED_INT, 0);
}

void RenderContext::onResize(GLFWwindow *window, int width, int height)
{
	smWindowSize.set(width, height);
	glViewport(0, 0, smWindowSize.x, smWindowSize.y);
	RenderEngine::getInstance().mCamera.get().onResize();
}
