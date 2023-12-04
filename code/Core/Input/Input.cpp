#include "Core/Input/Input.hpp"
#include "Core/Log/Log.hpp"
#include "Core/Profiler/Profiler.hpp"

void Input::init(const Ptr<IWindowInputAdapter>& windowInputAdapter)
{
	LOG_TRACE()

    mWindowInputAdapter = windowInputAdapter;

	smMouseCoordinates = Vector2();
	smLastMouseButtonPressed = -1;
	smLastKeyPressed = -1;
	smModifier = -1;
	smKeyJustPressed = false;
	smButtonJustPressed = false;
	smScroll = 0;
}

void Input::update()
{
	PROFILER_CPU()

	smKeyJustPressed = false;
	smButtonJustPressed = false;
	smScroll = 0;

	Vector2 newMouseCoordinates = mWindowInputAdapter->getMousePosition();

	if (!smMouseCoordinates.eq(newMouseCoordinates))
	{
		smMouseCoordinates.set(newMouseCoordinates);

		InputEventMouseMoved event;
		SEND_INPUT_EVENT(event);
	}

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
}

bool Input::isKeyPressedOnce(i32 key)
{
	return smKeyJustPressed && key == smLastKeyPressed;
}

bool Input::isKeyPressed(i32 key)
{
	return key == smLastKeyPressed;
}

bool Input::isModifierPressed(i32 modifier)
{
	return modifier == smModifier;
}

bool Input::isMouseButtonPressedOnce(i32 button)
{
	return smButtonJustPressed && button == smLastMouseButtonPressed;
}

bool Input::isMouseButtonPressed(i32 button)
{
	return button == smLastMouseButtonPressed;
}

const Vector2& Input::getMousePosition()
{
	return smMouseCoordinates;
}

f32 Input::getScroll()
{
	return smScroll;
}

void Input::clearMouseButton()
{
	smLastMouseButtonPressed = -1;
	smModifier = -1;
	smButtonJustPressed = false;
}

void Input::clearKey()
{
	smLastKeyPressed = -1;
	smModifier = -1;
	smKeyJustPressed = false;
}

