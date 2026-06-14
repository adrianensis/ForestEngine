#include "Window/Window.hpp"
#include "GLFW/glfw3.h"
#include "Input/InputEvents.hpp"
#include "Core/Event/EventsManager.hpp"
#include "Core/System/SystemsManager.hpp"
#include "GPU/Core/GPUContext.hpp"

namespace Window
{
	
GLFWwindow* Window::getGlfwWindow() const 
{
    return mGLFWWindow;
}

std::vector<const char*> Window::getRequiredExtensions() const
{
	unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

WindowSize Window::getWindowSize() const
{
	return mWindowData.mWindowSize;
}

float Window::getAspectRatio() const
{
	return mWindowData.mWindowSize.x / mWindowData.mWindowSize.y;
}

void Window::init(int id, const WindowData& windowData)
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
    glfwSetFramebufferSizeCallback(mGLFWWindow, &this->onResizeGLFW);
}

bool Window::isClosed() const
{
	return glfwWindowShouldClose(mGLFWWindow);
}

void Window::swap()
{
	// https://www.khronos.org/opengl/wiki/Common_Mistakes
	// section: glFinish and glFlush
	//glFlush();
	// glfwSwapBuffers(mGLFWWindow);
}

void Window::terminate()
{
	glfwDestroyWindow(mGLFWWindow);
	glfwTerminate();
}

void Window::setCursorVisibility(bool visible)
{
    glfwSetInputMode(mGLFWWindow, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void Window::onResize(GLFWwindow *window, int width, int height)
{
	mWindowData.mWindowSize = WindowSize{(float)width, (float)height};
	waitUntilNotMinimized();

	FOR_ARRAY(i, mWindowListeners)
	{
		mWindowListeners[i]->onResize(this);
	}
}

void Window::onResizeGLFW(GLFWwindow *windowGLFW, int width, int height)
{
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowGLFW));
    window->onResize(windowGLFW, width, height);
}

void Window::keyCallbackGLFW(GLFWwindow *windowGLFW, int key, int scancode, int action, int mods)
{
    Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowGLFW));
    window->keyCallback(key, scancode, action, mods);
}

void Window::mouseButtonCallbackGLFW(GLFWwindow *windowGLFW, int button, int action, int mods)
{
    Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowGLFW));
    window->mouseButtonCallback(button, action, mods);
}

void Window::scrollCallbackGLFW(GLFWwindow *windowGLFW, double xoffset, double yoffset)
{
    Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowGLFW));
    window->scrollCallback(xoffset, yoffset);
}

void Window::charCallbackGLFW(GLFWwindow *windowGLFW, unsigned int codepoint)
{
    Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowGLFW));
    window->charCallback(codepoint);
}

void Window::cursorPositionCallbackGLFW(GLFWwindow *windowGLFW, double x, double y)
{
    Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowGLFW));
    window->cursorPositionCallback(x, y);
}

void Window::keyCallback(int key, int scancode, int action, int mods)
{
	mInput->mModifier = mods;

	switch (action)
	{
		case GLFW_PRESS:
		{
			mInput->mLastKeyPressed = key;
			mInput->mKeyJustPressed = true;

			switch (key)
			{
				case GLFW_KEY_ENTER:
				{
					Input::InputEventKeyEnter event;
					GET_SYSTEM(Event::EventsManager).send<Input::InputEventKeyEnter>(nullptr, this, &event);
					break;
				}
				case GLFW_KEY_ESCAPE:
				{
					Input::InputEventKeyEsc event;
					GET_SYSTEM(Event::EventsManager).send<Input::InputEventKeyEsc>(nullptr, this, &event);
					break;
				}
				case GLFW_KEY_DELETE:
				{
					Input::InputEventKeyDelete event;
					GET_SYSTEM(Event::EventsManager).send<Input::InputEventKeyDelete>(nullptr, this, &event);
					break;
				}
				case GLFW_KEY_BACKSPACE:
				{
					Input::InputEventKeyBackspace event;
					GET_SYSTEM(Event::EventsManager).send<Input::InputEventKeyBackspace>(nullptr, this, &event);
					break;
				}
				case GLFW_KEY_TAB:
				{
					Input::InputEventKeyTab event;
					GET_SYSTEM(Event::EventsManager).send<Input::InputEventKeyTab>(nullptr, this, &event);
					break;
				}
				case GLFW_KEY_UP:
				case GLFW_KEY_DOWN:
				case GLFW_KEY_LEFT:
				case GLFW_KEY_RIGHT:
				{
					Input::InputEventKeyArrow event;
					event.mArrowButton = key;
					GET_SYSTEM(Event::EventsManager).send<Input::InputEventKeyArrow>(nullptr, this, &event);
					break;
				}
				default:
				{
					Input::InputEventKeyPressed event;
					event.mKey = key;
					event.mMods = mods;
					GET_SYSTEM(Event::EventsManager).send<Input::InputEventKeyPressed>(nullptr, this, &event);
					break;
				}
			}
			break;
		}
		case GLFW_RELEASE:
		{
			Input::InputEventKeyReleased event;
			event.mKey = key;
			event.mMods = mods;
			GET_SYSTEM(Event::EventsManager).send<Input::InputEventKeyReleased>(nullptr, this, &event);

			mInput->clearKey();
			break;
		}
		case GLFW_REPEAT:
		{
			Input::InputEventKeyHold event;
			event.mKey = key;
			event.mMods = mods;
			GET_SYSTEM(Event::EventsManager).send<Input::InputEventKeyHold>(nullptr, this, &event);

			break;
		}
	}
}

void Window::mouseButtonCallback(int button, int action, int mods)
{
	mInput->mModifier = mods;

	switch (action)
	{
		case GLFW_PRESS:
		{
			mInput->mLastMouseButtonPressed = button;
			mInput->mButtonJustPressed = true;

			Input::InputEventMouseButtonPressed event;
			event.mButton = button;
			event.mMods = mods;
			GET_SYSTEM(Event::EventsManager).send<Input::InputEventMouseButtonPressed>(nullptr, this, &event);
			
			break;
		}
		case GLFW_RELEASE:
		{
			Input::InputEventMouseButtonReleased event;
			event.mButton = button;
			event.mMods = mods;
			
            mInput->clearMouseButton();

			GET_SYSTEM(Event::EventsManager).send<Input::InputEventMouseButtonReleased>(nullptr, this, &event);

			break;
		}
	}
}

void Window::scrollCallback(double xoffset, double yoffset)
{
	mInput->mScroll = yoffset;

	Input::InputEventScroll event;
	event.mScroll = yoffset;
	GET_SYSTEM(Event::EventsManager).send<Input::InputEventScroll>(nullptr, this, &event);
}

void Window::charCallback(unsigned int codepoint)
{
	Input::InputEventChar event;
	event.mChar = (char)codepoint;
	GET_SYSTEM(Event::EventsManager).send<Input::InputEventChar>(nullptr, this, &event);
}

void Window::cursorPositionCallback(double x, double y)
{
	mInput->mMouseCoordinates = processCursorPosition(x, y);
	Input::InputEventMouseMoved event;
	GET_SYSTEM(Event::EventsManager).send<Input::InputEventMouseMoved>(nullptr, this, &event);
}

Input::InputCursorPosition Window::getMousePosition() const
{
	// double mouseCoordX, mouseCoordY;
	// glfwGetCursorPos(mGLFWWindow, &mouseCoordX, &mouseCoordY);
	// Input::InputCursorPosition newMouseCoordinates = processCursorPosition(mouseCoordX, mouseCoordY);
    // return newMouseCoordinates;

	return mInput->mMouseCoordinates;
}

Input::InputCursorPosition Window::processCursorPosition(double x, double y) const
{
	double halfWindowSizeX = mWindowData.mWindowSize.x / 2.0;
	double halfWindowSizeY = mWindowData.mWindowSize.y / 2.0;

	x = x - halfWindowSizeX;
	y = halfWindowSizeY - y;

	Input::InputCursorPosition newMouseCoordinates{x / halfWindowSizeX, y / halfWindowSizeY};
	return newMouseCoordinates;
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

void Window::waitUntilNotMinimized() const
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

bool Window::isIconified() const
{
	return glfwGetWindowAttrib(mGLFWWindow, GLFW_ICONIFIED) == 1;
}

void Window::addWindowListener(IWindowListener* windowListener)
{
	mWindowListeners.push_back(windowListener);
}

VkSurfaceKHR Window::createSurface(GPUContext* gpuContext) const
{
	VkAllocationCallbacks* allocator = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if(glfwCreateWindowSurface(gpuContext->gpuVulkanInstance->getVkInstance(), getGlfwWindow(), allocator, (VkSurfaceKHR*) &surface) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Error creating surface!")
    }

    return surface;
}

};