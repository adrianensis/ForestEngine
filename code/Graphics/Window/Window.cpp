#include "Graphics/Window/Window.hpp"
#include "Graphics/RenderEngine.hpp"

Vector2 Window::getWindowSize()
{
	return mWindowSize;
}

f32 Window::getAspectRatio()
{
	return mWindowSize.x / mWindowSize.y;
}

void Window::init()
{
	LOG_TRACE()

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE); // https://stackoverflow.com/questions/50412575/is-there-a-way-to-remove-60-fps-cap-in-glfw

    #ifdef ENGINE_ENABLE_GPU_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    #endif

    GLFWmonitor* monitor =  glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
 
    // glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    // glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    // glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    // glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    
	// mWindowSize.set(1080, 720);
	mWindowSize.set(mode->width, mode->height);

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	mGLTFWindow = glfwCreateWindow(mWindowSize.x, mWindowSize.y, "Engine", /*monitor*/NULL, NULL);

	if (mGLTFWindow)
	{
		glfwMakeContextCurrent(mGLTFWindow);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			LOG("Failed to initialize GLAD");
		}
        
		glfwSwapInterval(0);

        #ifdef ENGINE_ENABLE_GPU_DEBUG
        GET_SYSTEM(GPUInterface).setupGPUErrorHandling();
        #endif

		GET_SYSTEM(GPUInterface).setClearColor(Vector3(1,1,1));
		GET_SYSTEM(GPUInterface).enableFlag(GL_DEPTH_TEST); // Enable depth testing
		GET_SYSTEM(GPUInterface).setDepthFunc(GL_LEQUAL);

		GET_SYSTEM(GPUInterface).clear();
	}
	else
	{
		LOG("Failed to create GLFW window");
		glfwTerminate();
	}

    glfwSetKeyCallback(mGLTFWindow, keyCallbackGLFW);
	glfwSetMouseButtonCallback(mGLTFWindow, mouseButtonCallbackGLFW);
	glfwSetScrollCallback(mGLTFWindow, scrollCallbackGLFW);
	glfwSetCharCallback(mGLTFWindow, charCallbackGLFW);
    glfwSetFramebufferSizeCallback(mGLTFWindow, onResizeGLFW);
}

bool Window::isClosed()
{
	return glfwWindowShouldClose(mGLTFWindow);
}

void Window::swap()
{
	// https://www.khronos.org/opengl/wiki/Common_Mistakes
	// section: glFinish and glFlush
	//glFlush();
	glfwSwapBuffers(mGLTFWindow);
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

void Window::onResize(GLFWwindow *window, i32 width, i32 height)
{
	mWindowSize.set(width, height);
	GET_SYSTEM(RenderEngine).onResize(width, height);
}

void Window::onResizeGLFW(GLFWwindow *window, i32 width, i32 height)
{
	GET_SYSTEM(Window).onResize(window, width, height);
}

void Window::keyCallbackGLFW(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods)
{
    GET_SYSTEM(Window).keyCallback(key, scancode, action, mods);
}

void Window::mouseButtonCallbackGLFW(GLFWwindow *window, i32 button, i32 action, i32 mods)
{
    GET_SYSTEM(Window).mouseButtonCallback(button, action, mods);
}

void Window::scrollCallbackGLFW(GLFWwindow *window, f64 xoffset, f64 yoffset)
{
    GET_SYSTEM(Window).scrollCallback(xoffset, yoffset);
}

void Window::charCallbackGLFW(GLFWwindow *window, u32 codepoint)
{
    GET_SYSTEM(Window).charCallback(codepoint);
}

void Window::keyCallback(i32 key, i32 scancode, i32 action, i32 mods)
{
	GET_SYSTEM(Input).smModifier = mods;

	switch (action)
	{
		case GLFW_PRESS:
		{
			GET_SYSTEM(Input).smLastKeyPressed = key;
			GET_SYSTEM(Input).smKeyJustPressed = true;

			switch (key)
			{
				case GLFW_KEY_ENTER:
				{
					InputEventKeyEnter event;
					SEND_INPUT_EVENT(event);
					break;
				}
				case GLFW_KEY_ESCAPE:
				{
					InputEventKeyEsc event;
					SEND_INPUT_EVENT(event);
					break;
				}
				case GLFW_KEY_DELETE:
				{
					InputEventKeyDelete event;
					SEND_INPUT_EVENT(event);
					break;
				}
				case GLFW_KEY_BACKSPACE:
				{
					InputEventKeyBackspace event;
					SEND_INPUT_EVENT(event);
					break;
				}
				case GLFW_KEY_TAB:
				{
					InputEventKeyTab event;
					SEND_INPUT_EVENT(event);
					break;
				}
				case GLFW_KEY_UP:
				case GLFW_KEY_DOWN:
				case GLFW_KEY_LEFT:
				case GLFW_KEY_RIGHT:
				{
					InputEventKeyArrow event;
					event.mArrowButton = key;
					SEND_INPUT_EVENT(event);
					break;
				}
				default:
				{
					InputEventKeyPressed event;
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
			InputEventKeyReleased event;
			event.mKey = key;
			event.mMods = mods;
			SEND_INPUT_EVENT(event);

			GET_SYSTEM(Input).clearKey();
			break;
		}
		case GLFW_REPEAT:
		{
			InputEventKeyHold event;
			event.mKey = key;
			event.mMods = mods;
			SEND_INPUT_EVENT(event);

			break;
		}
	}
}

void Window::mouseButtonCallback(i32 button, i32 action, i32 mods)
{
	GET_SYSTEM(Input).smModifier = mods;

	switch (action)
	{
		case GLFW_PRESS:
		{
			GET_SYSTEM(Input).smLastMouseButtonPressed = button;
			GET_SYSTEM(Input).smButtonJustPressed = true;

			InputEventMouseButtonPressed event;
			event.mButton = button;
			event.mMods = mods;
			SEND_INPUT_EVENT(event);
			break;
		}
		case GLFW_RELEASE:
		{
			InputEventMouseButtonReleased event;
			event.mButton = button;
			event.mMods = mods;
			
            GET_SYSTEM(Input).clearMouseButton();

			SEND_INPUT_EVENT(event);

			break;
		}
	}
}

void Window::scrollCallback(f64 xoffset, f64 yoffset)
{
	GET_SYSTEM(Input).smScroll = yoffset;

	InputEventScroll event;
	event.mScroll = yoffset;
	SEND_INPUT_EVENT(event);
}

void Window::charCallback(u32 codepoint)
{
	InputEventChar event;
	event.mChar = (char)codepoint;
	SEND_INPUT_EVENT(event);
}

Vector2 Window::getMousePosition() const
{
	f64 mouseCoordX, mouseCoordY;

	glfwGetCursorPos(mGLTFWindow, &mouseCoordX, &mouseCoordY);

	f64 halfWindowSizeX = mWindowSize.x / 2.0;
	f64 halfWindowSizeY = mWindowSize.y / 2.0;

	mouseCoordX = mouseCoordX - halfWindowSizeX;
	mouseCoordY = halfWindowSizeY - mouseCoordY;

	Vector2 newMouseCoordinates(mouseCoordX / halfWindowSizeX, mouseCoordY / halfWindowSizeY);

    return newMouseCoordinates;
}

void Window::pollEvents() const
{
    glfwPollEvents();
}