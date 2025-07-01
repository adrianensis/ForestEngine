#pragma once

#include "Core/Events/EventsManager.hpp"

NS_BEGIN(Input)

#define CLASS_EVENT_SPECIFIC_KEY(KeyName) class InputEventKey##KeyName: public Core::Event {  }; REGISTER_CLASS(InputEventKey##KeyName);

CLASS_EVENT_SPECIFIC_KEY(Enter)
CLASS_EVENT_SPECIFIC_KEY(Esc)
CLASS_EVENT_SPECIFIC_KEY(Tab)
CLASS_EVENT_SPECIFIC_KEY(Delete)
CLASS_EVENT_SPECIFIC_KEY(Backspace)

class InputEventKeyArrow: public Core::Event
{
public:
    Core::u32 mArrowButton = 0;
};
REGISTER_CLASS(InputEventKeyArrow);

class InputEventMouseMoved: public Core::Event
{
};
REGISTER_CLASS(InputEventMouseMoved);

class InputEventMouseButtonPressed: public Core::Event
{
public:
    Core::u32 mButton = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventMouseButtonPressed);

class InputEventMouseButtonHold: public Core::Event
{
public:
    Core::u32 mButton = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventMouseButtonHold);

class InputEventMouseButtonReleased: public Core::Event
{
public:
    Core::u32 mButton = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventMouseButtonReleased);

class InputEventKeyPressed: public Core::Event
{
public:
    Core::u32 mKey = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventKeyPressed);

class InputEventKeyHold: public Core::Event
{
public:
    Core::u32 mKey = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventKeyHold);

class InputEventKeyReleased: public Core::Event
{
public:
    Core::u32 mKey = 0;
    Core::u32 mMods = 0;
};
REGISTER_CLASS(InputEventKeyReleased);

class InputEventScroll: public Core::Event
{
public:
    Core::f32 mScroll = 0.0f;
};
REGISTER_CLASS(InputEventScroll);

class InputEventChar: public Core::Event
{
public:
    char mChar;
};
REGISTER_CLASS(InputEventChar);

NS_END