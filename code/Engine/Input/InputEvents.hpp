#pragma once

#include "Engine/Events/EventsManager.hpp"

NS_BEGIN(Input)

#define CLASS_EVENT_SPECIFIC_KEY(KeyName) class InputEventKey##KeyName: public Event::Event {  }; REGISTER_CLASS(InputEventKey##KeyName);

CLASS_EVENT_SPECIFIC_KEY(Enter)
CLASS_EVENT_SPECIFIC_KEY(Esc)
CLASS_EVENT_SPECIFIC_KEY(Tab)
CLASS_EVENT_SPECIFIC_KEY(Delete)
CLASS_EVENT_SPECIFIC_KEY(Backspace)

class InputEventKeyArrow: public Event::Event
{
public:
    Core::u32 mArrowButton = 0;
};
REGISTER_CLASS(InputEventKeyArrow);

class InputEventMouseMoved: public Event::Event
{
};
REGISTER_CLASS(InputEventMouseMoved);

class InputEventMouseButtonPressed: public Event::Event
{
public:
    Core::u32 mButton = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventMouseButtonPressed);

class InputEventMouseButtonHold: public Event::Event
{
public:
    Core::u32 mButton = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventMouseButtonHold);

class InputEventMouseButtonReleased: public Event::Event
{
public:
    Core::u32 mButton = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventMouseButtonReleased);

class InputEventKeyPressed: public Event::Event
{
public:
    Core::u32 mKey = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventKeyPressed);

class InputEventKeyHold: public Event::Event
{
public:
    Core::u32 mKey = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventKeyHold);

class InputEventKeyReleased: public Event::Event
{
public:
    Core::u32 mKey = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventKeyReleased);

class InputEventScroll: public Event::Event
{
public:
    Core::f32 mScroll = 0.0f;
};
REGISTER_CLASS(InputEventScroll);

class InputEventChar: public Event::Event
{
public:
    char mChar;
};
REGISTER_CLASS(InputEventChar);

NS_END