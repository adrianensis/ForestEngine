#include "Input/Input.hpp"
#include "Core/Event/EventsManager.hpp"
#include "Core/System/SystemsManager.hpp"

NS_BEGIN(Input)
void Input::init()
{
	smMouseCoordinates = InputCursorPosition();
	smLastMouseButtonPressed = INVALID_INDEX;
	smLastKeyPressed = INVALID_INDEX;
	smModifier = INVALID_INDEX;
	smKeyJustPressed = false;
	smButtonJustPressed = false;
	smScroll = 0;
}

void Input::setWindowInputAdapter(IWindowInputAdapter* windowInputAdapter)
{
	mWindowInputAdapter = windowInputAdapter;
	windowInputAdapter->setInput(this);
}

void Input::update()
{
	smKeyJustPressed = false;
	smButtonJustPressed = false;
	smScroll = 0;

	assert(mWindowInputAdapter && "mWindowInputAdapter is NULL!");

	InputCursorPosition newMouseCoordinates = mWindowInputAdapter->getMousePosition();

	// if (!smMouseCoordinates.eq(newMouseCoordinates))
	{
		smMouseCoordinates = newMouseCoordinates;

		InputEventMouseMoved event;
		GET_SYSTEM(Event::EventsManager).send<InputEventMouseMoved>(nullptr, this, &event);
	}

	if(smLastMouseButtonPressed != -1)
	{
		InputEventMouseButtonHold event;
		event.mButton = smLastMouseButtonPressed;
		event.mMods = smModifier;
		GET_SYSTEM(Event::EventsManager).send<InputEventMouseButtonHold>(nullptr, this, &event);
	}

	if(smLastKeyPressed != -1)
	{
		InputEventKeyHold event;
		event.mKey = smLastKeyPressed;
		event.mMods = smModifier;
		GET_SYSTEM(Event::EventsManager).send<InputEventKeyHold>(nullptr, this, &event);
	}
}

bool Input::isKeyPressedOnce(int key)
{
	return smKeyJustPressed && key == smLastKeyPressed;
}

bool Input::isKeyPressed(int key)
{
	return key == smLastKeyPressed;
}

bool Input::isModifierPressed(int modifier)
{
	return modifier == smModifier;
}

bool Input::isMouseButtonPressedOnce(int button)
{
	return smButtonJustPressed && button == smLastMouseButtonPressed;
}

bool Input::isMouseButtonPressed(int button)
{
	return button == smLastMouseButtonPressed;
}

const InputCursorPosition& Input::getMousePosition()
{
	return smMouseCoordinates;
}

float Input::getScroll()
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