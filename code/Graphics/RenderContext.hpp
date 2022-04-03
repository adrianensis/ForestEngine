#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"

#include <stddef.h>
#endif

class Camera;

class RenderContext
{

public:
	friend class Input;

	CPP static Vector2 getWindowSize()
	{
		return smWindowSize;
	}

	CPP static f32 getAspectRatio()
	{
		return smWindowSize.x / smWindowSize.y;
	}

	CPP static void init()
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

			glClearColor(0,0,0,1);
			glEnable(GL_DEPTH_TEST); // Enable depth testing
			glDepthFunc(GL_LEQUAL);	 // Near things obscure far things
			glEnable(GL_CULL_FACE);	 // BACK by default
			glCullFace(GL_BACK);

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

	CPP static bool isClosed()
	{
		return glfwWindowShouldClose(smWindow);
	}

	CPP static void clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color and the depth buffer
	}

	CPP static void swap()
	{
		//glFlush(); // https://stackoverflow.com/questions/50412575/is-there-a-way-to-remove-60-fps-cap-in-glfw
		glfwSwapBuffers(smWindow);
		clear();
	}

	CPP static void terminate()
	{
		glfwDestroyWindow(smWindow);
		glfwTerminate();
	}

	CPP static GLuint createVBO(u32 elementSize, u32 PropertyArrayIndex)
	{
		u32 VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// for vertices elementSize should be 3 (x,y,z), for colors 4 (r,g,b,a)
		enableProperty(PropertyArrayIndex);
		glVertexAttribPointer(PropertyArrayIndex, elementSize, GL_FLOAT, GL_FALSE, elementSize * sizeof(f32), (byte *)0);
		return VBO;
	}

	CPP static GLuint createVAO()
	{
		u32 VAO;
		glGenVertexArrays(1, &VAO);
		enableVAO(VAO);
		return VAO;
	}

	CPP static GLuint createEBO()
	{
		u32 EBO;
		glGenBuffers(1, &EBO);
		return EBO;
	}

	CPP static void resizeVBO(u32 VBO, u32 size, u32 drawMode = GL_DYNAMIC_DRAW)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * size, nullptr, drawMode);
	}

	CPP static void resizeEBO(u32 EBO, u32 size, u32 drawMode = GL_DYNAMIC_DRAW)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * size, nullptr, drawMode);
	}

	CPP static void setDataVBO(u32 VBO, const std::vector<f32> &data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(f32) * data.size(), data.data());
	}

	CPP static void setDataEBO(u32 EBO, const std::vector<u16> &data)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(u16) * data.size(), data.data());
	}

	CPP static void enableProperty(u32 PropertyArrayIndex)
	{
		glEnableVertexAttribArray(PropertyArrayIndex);
	}

	CPP static void disableProperty(u32 PropertyArrayIndex)
	{
		glDisableVertexAttribArray(PropertyArrayIndex);
	}

	CPP static void enableVAO(u32 VAO)
	{
		glBindVertexArray(VAO);
	}

	CPP static void drawElements(u32 indicesCount, u32 instancesCount, bool instanced)
	{
		if(instanced)
		{
			glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, 0, instancesCount);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, instancesCount * indicesCount, GL_UNSIGNED_SHORT, 0);
		}

		glBindVertexArray(0);
	}

	CPP static void drawLines(u32 linesCount)
	{
		glDrawElements(GL_LINES, linesCount * 2, GL_UNSIGNED_SHORT, 0);
	}

private:
	CPP static void onResize(GLFWwindow *window, int width, int height)
	{
		smWindowSize.set(width, height);
		glViewport(0, 0, smWindowSize.x, smWindowSize.y);
		RenderEngine::getInstance().getCamera().get().onResize();
	}
	
private:
	inline static GLFWwindow *smWindow = nullptr;
	inline static Vector2 smWindowSize;
	inline static Camera *mCamera = nullptr;
};