#include "Input/Input.hpp"
#include "Core/Event/EventsManager.hpp"
#include "Core/System/SystemsManager.hpp"

namespace Input
{
void IInput::init()
{
	mMouseCoordinates = InputCursorPosition();
	mLastMouseButtonPressed = INVALID_INDEX;
	mLastKeyPressed = INVALID_INDEX;
	mModifier = INVALID_INDEX;
	mKeyJustPressed = false;
	mButtonJustPressed = false;
	mScroll = 0;
}

void IInput::setWindowInputAdapter(IWindowInputAdapter* windowInputAdapter)
{
	mWindowInputAdapter = windowInputAdapter;
	windowInputAdapter->setInput(this);
}

void IInput::update()
{
	mKeyJustPressed = false;
	mButtonJustPressed = false;
	mScroll = 0;

	assert(mWindowInputAdapter && "mWindowInputAdapter is NULL!");

	InputCursorPosition newMouseCoordinates = mWindowInputAdapter->getMousePosition();

	// if (!mMouseCoordinates.eq(newMouseCoordinates))
	// {
	// 	mMouseCoordinates = newMouseCoordinates;

	// 	InputEventMouseMoved event;
	// 	GET_SYSTEM(Event::EventsManager).send<InputEventMouseMoved>(nullptr, this, &event);
	// }

	// if(mLastMouseButtonPressed != -1)
	// {
	// 	InputEventMouseButtonHold event;
	// 	event.mButton = mLastMouseButtonPressed;
	// 	event.mMods = mModifier;
	// 	GET_SYSTEM(Event::EventsManager).send<InputEventMouseButtonHold>(nullptr, this, &event);
	// }

	// if(mLastKeyPressed != -1)
	// {
	// 	InputEventKeyHold event;
	// 	event.mKey = mLastKeyPressed;
	// 	event.mMods = mModifier;
	// 	GET_SYSTEM(Event::EventsManager).send<InputEventKeyHold>(nullptr, this, &event);
	// }
}

bool IInput::isKeyPressedOnce(int key)
{
	return mKeyJustPressed && key == mLastKeyPressed;
}

bool IInput::isKeyPressed(int key)
{
	return key == mLastKeyPressed;
}

bool IInput::isModifierPressed(int modifier)
{
	return modifier == mModifier;
}

bool IInput::isMouseButtonPressedOnce(int button)
{
	return mButtonJustPressed && button == mLastMouseButtonPressed;
}

bool IInput::isMouseButtonPressed(int button)
{
	return button == mLastMouseButtonPressed;
}

const InputCursorPosition& IInput::getMousePosition()
{
	return mMouseCoordinates;
}

float IInput::getScroll()
{
	return mScroll;
}

void IInput::clearMouseButton()
{
	mLastMouseButtonPressed = INVALID_INDEX;
	mModifier = INVALID_INDEX;
	mButtonJustPressed = false;
}

void IInput::clearKey()
{
	mLastKeyPressed = INVALID_INDEX;
	mModifier = INVALID_INDEX;
	mKeyJustPressed = false;
}

};