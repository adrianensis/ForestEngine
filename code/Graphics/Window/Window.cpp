#include "Graphics/Window/Window.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"

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
	TRACE()

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE); // https://stackoverflow.com/questions/50412575/is-there-a-way-to-remove-60-fps-cap-in-glfw

	mWindowSize.set(1080, 720);

	mGLTFWindow = glfwCreateWindow(mWindowSize.x, mWindowSize.y, "Engine", NULL, NULL);

	if (mGLTFWindow)
	{
		glfwMakeContextCurrent(mGLTFWindow);

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
        i32 gl_MAX_VERTEX_UNIFORM_VECTORS_IN_BYTES = gl_MAX_VERTEX_UNIFORM_VECTORS * 4;
        VAR(gl_MAX_VERTEX_UNIFORM_VECTORS_IN_BYTES);

        ECHO("UNIFORM BUFFER");
        i32 gl_MAX_UNIFORM_BUFFER_BINDINGS;
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &gl_MAX_UNIFORM_BUFFER_BINDINGS);
        VAR(gl_MAX_UNIFORM_BUFFER_BINDINGS);
        i32 gl_MAX_UNIFORM_BLOCK_SIZE;
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &gl_MAX_UNIFORM_BLOCK_SIZE);
        VAR(gl_MAX_UNIFORM_BLOCK_SIZE);
        // vectors / 4 components = matrices
        i32 gl_MAX_UNIFORM_BLOCK_SIZE_IN_MATRICES_4x4 = (gl_MAX_UNIFORM_BLOCK_SIZE/4/4);
        VAR(gl_MAX_UNIFORM_BLOCK_SIZE_IN_MATRICES_4x4);

        ECHO("SHADER STORAGE");
        i32 gl_MAX_SHADER_STORAGE_BUFFER_BINDINGS;
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &gl_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
        VAR(gl_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
        i32 gl_MAX_SHADER_STORAGE_BLOCK_SIZE;
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &gl_MAX_SHADER_STORAGE_BLOCK_SIZE);
        VAR(gl_MAX_SHADER_STORAGE_BLOCK_SIZE);
        // vectors / 4 components = matrices
        i32 gl_MAX_SHADER_STORAGE_BLOCK_SIZE_IN_MATRICES_4x4 = (gl_MAX_SHADER_STORAGE_BLOCK_SIZE/4/4);
        VAR(gl_MAX_SHADER_STORAGE_BLOCK_SIZE_IN_MATRICES_4x4);

		glfwSwapInterval(0);


		glClearColor(0,0.3,0.3,1);
		glEnable(GL_DEPTH_TEST); // Enable depth testing
		glDepthFunc(GL_LEQUAL);
		// glEnable(GL_CULL_FACE);	 // BACK by default
		// glCullFace(GL_BACK);
		// glFrontFace(GL_CCW);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		GET_SYSTEM(GPUInterface).clear();
	}
	else
	{
		ECHO("Failed to create GLFW window");
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

void Window::onResize(GLFWwindow *window, int width, int height)
{
	mWindowSize.set(width, height);
	glViewport(0, 0, mWindowSize.x, mWindowSize.y);
	GET_SYSTEM(RenderEngine).mCamera->onResize();
}

void Window::onResizeGLFW(GLFWwindow *window, int width, int height)
{
	GET_SYSTEM(Window).onResize(window, width, height);
}

void Window::keyCallbackGLFW(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    GET_SYSTEM(Window).keyCallback(key, scancode, action, mods);
}

void Window::mouseButtonCallbackGLFW(GLFWwindow *window, int button, int action, int mods)
{
    GET_SYSTEM(Window).mouseButtonCallback(button, action, mods);
}

void Window::scrollCallbackGLFW(GLFWwindow *window, double xoffset, double yoffset)
{
    GET_SYSTEM(Window).scrollCallback(xoffset, yoffset);
}

void Window::charCallbackGLFW(GLFWwindow *window, unsigned int codepoint)
{
    GET_SYSTEM(Window).charCallback(codepoint);
}

void Window::keyCallback(int key, int scancode, int action, int mods)
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

void Window::mouseButtonCallback(int button, int action, int mods)
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
			SEND_INPUT_EVENT(event);

			GET_SYSTEM(Input).clearMouseButton();
			break;
		}
	}
}

void Window::scrollCallback(double xoffset, double yoffset)
{
	GET_SYSTEM(Input).smScroll = yoffset;

	InputEventScroll event;
	event.mScroll = yoffset;
	SEND_INPUT_EVENT(event);
}

void Window::charCallback(unsigned int codepoint)
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