#include "Engine/Input/Input.hpp"
#include "Core/Log/Log.hpp"
#include "Core/Profiler/Profiler.hpp"

NS_BEGIN(Input)
void Input::init()
{
	smMouseCoordinates = Vector2();
	smLastMouseButtonPressed = INVALID_INDEX;
	smLastKeyPressed = INVALID_INDEX;
	smModifier = INVALID_INDEX;
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

	CHECK_MSG(mWindowInputAdapter, "mWindowInputAdapter is NULL!")

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

bool Input::isKeyPressedOnce(Core::i32 key)
{
	return smKeyJustPressed && key == smLastKeyPressed;
}

bool Input::isKeyPressed(Core::i32 key)
{
	return key == smLastKeyPressed;
}

bool Input::isModifierPressed(Core::i32 modifier)
{
	return modifier == smModifier;
}

bool Input::isMouseButtonPressedOnce(Core::i32 button)
{
	return smButtonJustPressed && button == smLastMouseButtonPressed;
}

bool Input::isMouseButtonPressed(Core::i32 button)
{
	return button == smLastMouseButtonPressed;
}

const Vector2& Input::getMousePosition()
{
	return smMouseCoordinates;
}

Core::f32 Input::getScroll()
{
	return smScroll;
}

void Input::clearMouseButton()
{
	smLastMouseButtonPressed = INVALID_INDEX;
	smModifier = INVALID_INDEX;
	smButtonJustPressed = false;
}

void Input::clearKey()
{
	smLastKeyPressed = INVALID_INDEX;
	smModifier = INVALID_INDEX;
	smKeyJustPressed = false;
}

NS_END