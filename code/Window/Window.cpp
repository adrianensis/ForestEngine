#include "Window/Window.hpp"
#include "Core/Profiler/Profiler.hpp"
#include "Core/System/SystemsManager.hpp"
#include "GPU/Core/GPUContext.hpp"

NS_BEGIN(Window)
GLFWwindow* Window::getGlfwWindow() const 
{
    return mGLTFWindow;
}

std::vector<const char*> Window::getRequiredExtensions() const
{
	Core::u32 glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

Maths::Vector2 Window::getWindowSize() const
{
	return mWindowData.mWindowSize;
}

Core::f32 Window::getAspectRatio() const
{
	return mWindowData.mWindowSize.x / mWindowData.mWindowSize.y;
}

void Window::init(Core::i32 id, const WindowData& windowData)
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
	    mWindowData.mWindowSize.set(mode->width, mode->height);
    }
    
    // glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	mGLTFWindow = glfwCreateWindow(mWindowData.mWindowSize.x, mWindowData.mWindowSize.y, mWindowData.mTitle.get().c_str(), /*monitor*/NULL, NULL);

    glfwSetWindowUserPointer(mGLTFWindow, reinterpret_cast<void *>(this));

    if (mGLTFWindow)
    {
        glfwMakeContextCurrent(mGLTFWindow);

//        GET_SYSTEM(GPUInterface).loadAPI();
        
        glfwSwapInterval(0);

        #ifdef ENGINE_ENABLE_GPU_DEBUG
//        GET_SYSTEM(GPUInterface).setupGPUErrorHandling();
        #endif

//        GET_SYSTEM(GPUInterface).setClearColor(Maths::Vector3(28.0/256.0, 28.0/256.0, 28.0/256.0));
    }
    else
    {
        LOG("Failed to create GLFW window");
        glfwTerminate();
		return;
    }

	glfwSetCursorPos(mGLTFWindow, mWindowData.mWindowSize.x / 2.0f, mWindowData.mWindowSize.y / 2.0f);

    glfwSetKeyCallback(mGLTFWindow, keyCallbackGLFW);
	glfwSetMouseButtonCallback(mGLTFWindow, mouseButtonCallbackGLFW);
	glfwSetScrollCallback(mGLTFWindow, scrollCallbackGLFW);
	glfwSetCharCallback(mGLTFWindow, charCallbackGLFW);
    glfwSetFramebufferSizeCallback(mGLTFWindow, &this->onResizeGLFW);
}

bool Window::isClosed() const
{
	return glfwWindowShouldClose(mGLTFWindow);
}

void Window::swap()
{
	// https://www.khronos.org/opengl/wiki/Common_Mistakes
	// section: glFinish and glFlush
	//glFlush();
	// glfwSwapBuffers(mGLTFWindow);
}

void Window::terminate()
{
	glfwDestroyWindow(mGLTFWindow);
	glfwTerminate();
}

void Window::setCursorVisibility(bool visible)
{
    glfwSetInputMode(mGLTFWindow, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void Window::onResize(GLFWwindow *window, Core::i32 width, Core::i32 height)
{
	mWindowData.mWindowSize.set(width, height);
	waitUntilNotMinimized();
	// GET_SYSTEM(RenderEngine).onResize(width, height);

	FOR_ARRAY(i, mWindowListeners)
	{
		mWindowListeners[i]->onResize();
	}
}

void Window::onResizeGLFW(GLFWwindow *windowGLFW, Core::i32 width, Core::i32 height)
{
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowGLFW));
    window->onResize(windowGLFW, width, height);
}

void Window::keyCallbackGLFW(GLFWwindow *windowGLFW, Core::i32 key, Core::i32 scancode, Core::i32 action, Core::i32 mods)
{
    Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowGLFW));
    window->keyCallback(key, scancode, action, mods);
}

void Window::mouseButtonCallbackGLFW(GLFWwindow *windowGLFW, Core::i32 button, Core::i32 action, Core::i32 mods)
{
    Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowGLFW));
    window->mouseButtonCallback(button, action, mods);
}

void Window::scrollCallbackGLFW(GLFWwindow *windowGLFW, Core::f64 xoffset, Core::f64 yoffset)
{
    Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowGLFW));
    window->scrollCallback(xoffset, yoffset);
}

void Window::charCallbackGLFW(GLFWwindow *windowGLFW, Core::u32 codepoint)
{
    Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowGLFW));
    window->charCallback(codepoint);
}

void Window::keyCallback(Core::i32 key, Core::i32 scancode, Core::i32 action, Core::i32 mods)
{
	GET_SYSTEM(Input::Input).smModifier = mods;

	switch (action)
	{
		case GLFW_PRESS:
		{
			GET_SYSTEM(Input::Input).smLastKeyPressed = key;
			GET_SYSTEM(Input::Input).smKeyJustPressed = true;

			switch (key)
			{
				case GLFW_KEY_ENTER:
				{
					Input::InputEventKeyEnter event;
					SEND_INPUT_EVENT(event);
					break;
				}
				case GLFW_KEY_ESCAPE:
				{
					Input::InputEventKeyEsc event;
					SEND_INPUT_EVENT(event);
					break;
				}
				case GLFW_KEY_DELETE:
				{
					Input::InputEventKeyDelete event;
					SEND_INPUT_EVENT(event);
					break;
				}
				case GLFW_KEY_BACKSPACE:
				{
					Input::InputEventKeyBackspace event;
					SEND_INPUT_EVENT(event);
					break;
				}
				case GLFW_KEY_TAB:
				{
					Input::InputEventKeyTab event;
					SEND_INPUT_EVENT(event);
					break;
				}
				case GLFW_KEY_UP:
				case GLFW_KEY_DOWN:
				case GLFW_KEY_LEFT:
				case GLFW_KEY_RIGHT:
				{
					Input::InputEventKeyArrow event;
					event.mArrowButton = key;
					SEND_INPUT_EVENT(event);
					break;
				}
				default:
				{
					Input::InputEventKeyPressed event;
					event.mKey = key;
					event.mMods = mods;
					SEND_INPUT_EVENT(event);
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
			SEND_INPUT_EVENT(event);

			GET_SYSTEM(Input::Input).clearKey();
			break;
		}
		case GLFW_REPEAT:
		{
			Input::InputEventKeyHold event;
			event.mKey = key;
			event.mMods = mods;
			SEND_INPUT_EVENT(event);

			break;
		}
	}
}

void Window::mouseButtonCallback(Core::i32 button, Core::i32 action, Core::i32 mods)
{
	GET_SYSTEM(Input::Input).smModifier = mods;

	switch (action)
	{
		case GLFW_PRESS:
		{
			GET_SYSTEM(Input::Input).smLastMouseButtonPressed = button;
			GET_SYSTEM(Input::Input).smButtonJustPressed = true;

			Input::InputEventMouseButtonPressed event;
			event.mButton = button;
			event.mMods = mods;
			SEND_INPUT_EVENT(event);
			break;
		}
		case GLFW_RELEASE:
		{
			Input::InputEventMouseButtonReleased event;
			event.mButton = button;
			event.mMods = mods;
			
            GET_SYSTEM(Input::Input).clearMouseButton();

			SEND_INPUT_EVENT(event);

			break;
		}
	}
}

void Window::scrollCallback(Core::f64 xoffset, Core::f64 yoffset)
{
	GET_SYSTEM(Input::Input).smScroll = yoffset;

	Input::InputEventScroll event;
	event.mScroll = yoffset;
	SEND_INPUT_EVENT(event);
}

void Window::charCallback(Core::u32 codepoint)
{
	Input::InputEventChar event;
	event.mChar = (char)codepoint;
	SEND_INPUT_EVENT(event);
}

Maths::Vector2 Window::getMousePosition() const
{
	Core::f64 mouseCoordX, mouseCoordY;

	glfwGetCursorPos(mGLTFWindow, &mouseCoordX, &mouseCoordY);

	Core::f64 halfWindowSizeX = mWindowData.mWindowSize.x / 2.0;
	Core::f64 halfWindowSizeY = mWindowData.mWindowSize.y / 2.0;

	mouseCoordX = mouseCoordX - halfWindowSizeX;
	mouseCoordY = halfWindowSizeY - mouseCoordY;

	Maths::Vector2 newMouseCoordinates(mouseCoordX / halfWindowSizeX, mouseCoordY / halfWindowSizeY);

    return newMouseCoordinates;
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

void Window::waitUntilNotMinimized() const
{
	Maths::Vector2 size = getWindowSize();
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
	return glfwGetWindowAttrib(mGLTFWindow, GLFW_ICONIFIED) == 1;
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

NS_END