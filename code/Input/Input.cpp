#include "Input/Input.hpp"
#include "Core/Event/EventsManager.hpp"
#include "Core/Log/Log.hpp"
#include "Core/System/SystemsManager.hpp"
#include "Core/Profiler/Profiler.hpp"

NS_BEGIN(Input)
void Input::init()
{
	smMouseCoordinates = Maths::Vector2();
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
	PROFILER_CPU()

	smKeyJustPressed = false;
	smButtonJustPressed = false;
	smScroll = 0;

	CHECK_MSG(mWindowInputAdapter, "mWindowInputAdapter is NULL!")

	Maths::Vector2 newMouseCoordinates = mWindowInputAdapter->getMousePosition();

	if (!smMouseCoordinates.eq(newMouseCoordinates))
	{
		smMouseCoordinates.set(newMouseCoordinates);

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

const Maths::Vector2& Input::getMousePosition()
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