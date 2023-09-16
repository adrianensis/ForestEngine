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
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE); // https://stackoverflow.com/questions/50412575/is-there-a-way-to-remove-60-fps-cap-in-glfw

	smWindowSize.set(1080, 720);

	smWindow = glfwCreateWindow(smWindowSize.x, smWindowSize.y, "Engine", NULL, NULL);

	if (smWindow)
	{
		glfwMakeContextCurrent(smWindow);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			ECHO("Failed to initialize GLAD");
		}
        
        ECHO("UNIFORM");
        i32 gl_MAX_VERTEX_UNIFORM_VECTORS;
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &gl_MAX_VERTEX_UNIFORM_VECTORS);
        VAR(gl_MAX_VERTEX_UNIFORM_VECTORS);
        // vectors / 4 components = matrices
        i32 gl_MAX_VERTEX_UNIFORM_VECTORS_IN_MATRICES_4x4 = (gl_MAX_VERTEX_UNIFORM_VECTORS/4);
        VAR(gl_MAX_VERTEX_UNIFORM_VECTORS_IN_MATRICES_4x4);
        // matrices * 16 = floats
        // floats * 4 = bytes
        i32 gl_MAX_VERTEX_UNIFORM_VECTORS_IN_BYTES = gl_MAX_VERTEX_UNIFORM_VECTORS_IN_MATRICES_4x4*16*4;
        VAR(gl_MAX_VERTEX_UNIFORM_VECTORS_IN_BYTES);

        ECHO("UNIFORM BUFFER");
        i32 gl_MAX_UNIFORM_BUFFER_BINDINGS;
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &gl_MAX_UNIFORM_BUFFER_BINDINGS);
        VAR(gl_MAX_UNIFORM_BUFFER_BINDINGS);
        i32 gl_MAX_UNIFORM_BLOCK_SIZE;
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &gl_MAX_UNIFORM_BLOCK_SIZE);
        VAR(gl_MAX_UNIFORM_BLOCK_SIZE);

        ECHO("SHADER STORAGE");
        i32 gl_MAX_SHADER_STORAGE_BUFFER_BINDINGS;
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &gl_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
        VAR(gl_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
        i32 gl_MAX_SHADER_STORAGE_BLOCK_SIZE;
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &gl_MAX_SHADER_STORAGE_BLOCK_SIZE);
        VAR(gl_MAX_SHADER_STORAGE_BLOCK_SIZE);

		glfwSwapInterval(0);

		glfwSetFramebufferSizeCallback(smWindow, GLFWonResize);

		glClearColor(0,0.3,0.3,1);
		glEnable(GL_DEPTH_TEST); // Enable depth testing
		glDepthFunc(GL_LEQUAL);
		// glEnable(GL_CULL_FACE);	 // BACK by default
		// glCullFace(GL_BACK);
		// glFrontFace(GL_CCW);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		clear();
	}
	else
	{
		ECHO("Failed to create GLFW window");
		glfwTerminate();
	}
}

bool RenderContext::isClosed()
{
	return glfwWindowShouldClose(smWindow);
}

void RenderContext::clearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderContext::clearStencil()
{
	glClear(GL_STENCIL_BUFFER_BIT);
}

void RenderContext::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderContext::swap()
{
	// https://www.khronos.org/opengl/wiki/Common_Mistakes
	// section: glFinish and glFlush
	//glFlush();
	glfwSwapBuffers(smWindow);
}

void RenderContext::terminate()
{
	glfwDestroyWindow(smWindow);
	glfwTerminate();
}

void RenderContext::onResize(GLFWwindow *window, int width, int height)
{
	smWindowSize.set(width, height);
	glViewport(0, 0, smWindowSize.x, smWindowSize.y);
	GET_SYSTEM(RenderEngine).mCamera->onResize();
}

void RenderContext::GLFWonResize(GLFWwindow *window, int width, int height)
{
	GET_SYSTEM(RenderContext).onResize(window, width, height);
}
