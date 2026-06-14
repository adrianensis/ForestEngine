#include "Input/Input.hpp"

namespace Input
{
void IInput::init()
{
	mMouseCoordinates = InputCursorPosition();
	mLastMouseButtonPressed = -1;
	mLastKeyPressed = -1;
	mModifier = -1;
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

	// assert(mWindowInputAdapter && "mWindowInputAdapter is NULL!");
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
	mLastMouseButtonPressed = -1;
	mModifier = -1;
	mButtonJustPressed = false;
}

void IInput::clearKey()
{
	mLastKeyPressed = -1;
	mModifier = -1;
	mKeyJustPressed = false;
}

};