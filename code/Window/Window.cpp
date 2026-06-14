#include "Window/Window.hpp"
#include "Window/WindowFramework.hpp"
#include "Core/Log/Log.hpp"

namespace Window
{
	
GLFWwindow* IWindow::getGlfwWindow() const 
{
    return mGLFWWindow;
}

std::vector<const char*> IWindow::getRequiredExtensions() const
{
	unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

WindowSize IWindow::getWindowSize() const
{
	return mWindowData.mWindowSize;
}

float IWindow::getAspectRatio() const
{
	return mWindowData.mWindowSize.x / mWindowData.mWindowSize.y;
}

void IWindow::init(int id, const WindowData& windowData)
{
    mID = id;
    mWindowData = windowData;

	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	// glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE); // https://stackoverflow.com/questions/50412575/is-there-a-way-to-remove-60-fps-cap-in-glfw

    // glfwWindowHint(GLFW_SAMPLES, 8);

    // #ifdef ENGINE_ENABLE_GPU_DEBUG
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    // #endif
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWmonitor* monitor =  glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
 
    // glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    // glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    // glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    // glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    if(mWindowData.mFullScreen)
    {
	    mWindowData.mWindowSize = WindowSize{(float)mode->width, (float)mode->height};
    }
    
    // glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	mGLFWWindow = glfwCreateWindow(mWindowData.mWindowSize.x, mWindowData.mWindowSize.y, mWindowData.mTitle.c_str(), /*monitor*/NULL, NULL);

    glfwSetWindowUserPointer(mGLFWWindow, reinterpret_cast<void *>(this));

    if (mGLFWWindow)
    {
        glfwMakeContextCurrent(mGLFWWindow);
        glfwSwapInterval(0);
    }
    else
    {
        LOG("Failed to create GLFW window");
        glfwTerminate();
		return;
    }

	glfwSetCursorPos(mGLFWWindow, mWindowData.mWindowSize.x / 2.0f, mWindowData.mWindowSize.y / 2.0f);

    glfwSetKeyCallback(mGLFWWindow, keyCallbackGLFW);
	glfwSetMouseButtonCallback(mGLFWWindow, mouseButtonCallbackGLFW);
	glfwSetScrollCallback(mGLFWWindow, scrollCallbackGLFW);
	glfwSetCharCallback(mGLFWWindow, charCallbackGLFW);
	glfwSetCursorPosCallback(mGLFWWindow, cursorPositionCallbackGLFW);
    glfwSetFramebufferSizeCallback(mGLFWWindow, onResizeGLFW);
}

bool IWindow::isClosed() const
{
	return glfwWindowShouldClose(mGLFWWindow);
}

void IWindow::swap()
{
	// https://www.khronos.org/opengl/wiki/Common_Mistakes
	// section: glFinish and glFlush
	//glFlush();
	// glfwSwapBuffers(mGLFWWindow);
}

void IWindow::terminate()
{
	glfwDestroyWindow(mGLFWWindow);
	glfwTerminate();
}

void IWindow::update()
{
	onUpdate();
	pollEvents();
}

void IWindow::setCursorVisibility(bool visible)
{
    glfwSetInputMode(mGLFWWindow, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void IWindow::onResizeCallback(int width, int height)
{
	mWindowData.mWindowSize = WindowSize{(float)width, (float)height};
	waitUntilNotMinimized();

	FOR_ARRAY(i, mWindowListeners)
	{
		mWindowListeners[i]->onResize(this);
	}
}

void IWindow::onResizeGLFW(GLFWwindow *windowGLFW, int width, int height)
{
	IWindow* window = reinterpret_cast<IWindow*>(glfwGetWindowUserPointer(windowGLFW));
    window->onResizeCallback(width, height);
}

void IWindow::keyCallbackGLFW(GLFWwindow *windowGLFW, int key, int scancode, int action, int mods)
{
    IWindow* window = reinterpret_cast<IWindow*>(glfwGetWindowUserPointer(windowGLFW));
    window->onKeyCallback(key, scancode, action, mods);
}

void IWindow::mouseButtonCallbackGLFW(GLFWwindow *windowGLFW, int button, int action, int mods)
{
    IWindow* window = reinterpret_cast<IWindow*>(glfwGetWindowUserPointer(windowGLFW));
    window->onMouseButtonCallback(button, action, mods);
}

void IWindow::scrollCallbackGLFW(GLFWwindow *windowGLFW, double xoffset, double yoffset)
{
    IWindow* window = reinterpret_cast<IWindow*>(glfwGetWindowUserPointer(windowGLFW));
    window->onScrollCallback(xoffset, yoffset);
}

void IWindow::charCallbackGLFW(GLFWwindow *windowGLFW, unsigned int codepoint)
{
    IWindow* window = reinterpret_cast<IWindow*>(glfwGetWindowUserPointer(windowGLFW));
    window->onCharCallback(codepoint);
}

void IWindow::cursorPositionCallbackGLFW(GLFWwindow *windowGLFW, double x, double y)
{
    IWindow* window = reinterpret_cast<IWindow*>(glfwGetWindowUserPointer(windowGLFW));
    window->onCursorPositionCallback(x, y);
}

Input::InputCursorPosition IWindow::processCursorPosition(double x, double y) const
{
	double halfWindowSizeX = mWindowData.mWindowSize.x / 2.0;
	double halfWindowSizeY = mWindowData.mWindowSize.y / 2.0;

	x = x - halfWindowSizeX;
	y = halfWindowSizeY - y;

	Input::InputCursorPosition newMouseCoordinates{x / halfWindowSizeX, y / halfWindowSizeY};
	return newMouseCoordinates;
}

void IWindow::pollEvents() const
{
    glfwPollEvents();
}

void IWindow::waitUntilNotMinimized() const
{
	WindowSize size = getWindowSize();
	int width = size.x;
	int height = size.y;

	bool iconified = isIconified();

	while (width == 0 || height == 0 || iconified) {
		size = getWindowSize();
		width = size.x;
		height = size.y;
		iconified = isIconified();
		glfwWaitEvents();
	}
}

bool IWindow::isIconified() const
{
	return glfwGetWindowAttrib(mGLFWWindow, GLFW_ICONIFIED) == 1;
}

void IWindow::addWindowListener(IWindowListener* windowListener)
{
	mWindowListeners.push_back(windowListener);
}

};