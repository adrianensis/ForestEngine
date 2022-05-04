#pragma once

#include "Core/ObjectBase.hpp"
#include "Core/Singleton.hpp"

#include "Graphics/RenderContext.hpp"
#include "Core/Maths/Vector2.hpp"

#include "Core/Input/InputEvents.hpp"

#ifdef CPP_INCLUDE
#include "Core/Input/Input.hpp"

#endif

class Input: public ObjectBase, public Singleton<Input>
{
	GENERATE_METADATA(Input)
public:
	Vector2 smMouseCoordinates;
	u32 smLastMouseButtonPressed;
	u32 smLastKeyPressed;
	u32 smModifier;
	bool smKeyJustPressed;
	bool smButtonJustPressed;
	f32 smScroll;

	CPP void init()
	{
		TRACE()

		smMouseCoordinates = Vector2();
		smLastMouseButtonPressed = -1;
		smLastKeyPressed = -1;
		smModifier = -1;
		smKeyJustPressed = false;
		smButtonJustPressed = false;
		smScroll = 0;

		glfwSetKeyCallback(RenderContext::smWindow, keyCallback);
		glfwSetMouseButtonCallback(RenderContext::smWindow, mouseButtonCallback);
		glfwSetScrollCallback(RenderContext::smWindow, scrollCallback);
		glfwSetCharCallback(RenderContext::smWindow, charCallback);
	}

	CPP void pollEvents()
	{
		PROFILER_TIMEMARK_START()

		smKeyJustPressed = false;
		smButtonJustPressed = false;
		smScroll = 0;

		f64 mouseCoordX, mouseCoordY;

		glfwGetCursorPos(RenderContext::smWindow, &mouseCoordX, &mouseCoordY);

		f64 halfWindowSizeX = RenderContext::smWindowSize.x / 2.0;
		f64 halfWindowSizeY = RenderContext::smWindowSize.y / 2.0;

		mouseCoordX = mouseCoordX - halfWindowSizeX;
		mouseCoordY = halfWindowSizeY - mouseCoordY;

		bool moved = false;
		Vector2 newMouseCoordinates(mouseCoordX / halfWindowSizeX, mouseCoordY / halfWindowSizeY);

		if (!smMouseCoordinates.eq(newMouseCoordinates))
		{
			smMouseCoordinates.set(newMouseCoordinates);

			InputEventMouseMoved event;
			SEND_INPUT_EVENT(event);
		}

		// HACK: implement Hold event since GLFW_REPEAT awaits a small time after GLFW_PRESS there's no GLFW_REPEAT for mouse
		if(smLastMouseButtonPressed != -1)
		{
			InputEventMouseButtonHold event;
			event.mButton = smLastMouseButtonPressed;
			event.mMods = smModifier;
			SEND_INPUT_EVENT(event);
		}

		if(smLastKeyPressed != -1)
		{
			InputEventKeyHold event;
			event.mKey = smLastKeyPressed;
			event.mMods = smModifier;
			SEND_INPUT_EVENT(event);
		}

		glfwPollEvents();

		PROFILER_TIMEMARK_END();
	}

	CPP bool isKeyPressedOnce(u32 key)
	{
		return smKeyJustPressed && key == smLastKeyPressed;
	}

	CPP bool isKeyPressed(u32 key)
	{
		return key == smLastKeyPressed;
	}

	CPP bool isModifierPressed(u32 modifier)
	{
		return modifier == smModifier;
	}

	CPP bool isMouseButtonPressedOnce(u32 button)
	{
		return smButtonJustPressed && button == smLastMouseButtonPressed;
	}

	CPP bool isMouseButtonPressed(u32 button)
	{
		return button == smLastMouseButtonPressed;
	}

	CPP const Vector2& getMousePosition()
	{
		return smMouseCoordinates;
	}

	CPP f32 getScroll()
	{
		return smScroll;
	}

	CPP void clearMouseButton()
	{
		smLastMouseButtonPressed = -1;
		smModifier = -1;
		smButtonJustPressed = false;
	}

	CPP void clearKey()
	{
		smLastKeyPressed = -1;
		smModifier = -1;
		smKeyJustPressed = false;
	}

	CPP void setCursorVisibility(bool visible)
	{
		glfwSetInputMode(RenderContext::smWindow, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}

private:
	CPP static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		Input::getInstance().smModifier = mods;

		switch (action)
		{
			case GLFW_PRESS:
			{
				Input::getInstance().smLastKeyPressed = key;
				Input::getInstance().smKeyJustPressed = true;

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

				Input::getInstance().clearKey();
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

	CPP static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
	{
		Input::getInstance().smModifier = mods;

		switch (action)
		{
			case GLFW_PRESS:
			{
				Input::getInstance().smLastMouseButtonPressed = button;
				Input::getInstance().smButtonJustPressed = true;

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

				Input::getInstance().clearMouseButton();
				break;
			}
		}
	}

	CPP static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
	{
		Input::getInstance().smScroll = yoffset;

		InputEventScroll event;
		event.mScroll = yoffset;
		SEND_INPUT_EVENT(event);
	}

	CPP static void charCallback(GLFWwindow *window, unsigned int codepoint)
	{
		InputEventChar event;
		event.mChar = (char)codepoint;
		SEND_INPUT_EVENT(event);
	}
};