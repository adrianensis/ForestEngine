#pragma once

#include "Engine/Events/EventsManager.hpp"

#define SEND_INPUT_EVENT(event) SEND_EVENT(nullptr, &GET_SYSTEM(Input), event);

#define CLASS_EVENT_SPECIFIC_KEY(KeyName) class InputEventKey##KeyName: public Event {  }; REGISTER_CLASS(InputEventKey##KeyName);

CLASS_EVENT_SPECIFIC_KEY(Enter)
CLASS_EVENT_SPECIFIC_KEY(Esc)
CLASS_EVENT_SPECIFIC_KEY(Tab)
CLASS_EVENT_SPECIFIC_KEY(Delete)
CLASS_EVENT_SPECIFIC_KEY(Backspace)

class InputEventKeyArrow: public Event
{
public:
    u32 mArrowButton = 0;
};
REGISTER_CLASS(InputEventKeyArrow);

class InputEventMouseMoved: public Event{
};
REGISTER_CLASS(InputEventMouseMoved);

class InputEventMouseButtonPressed: public Event
{
public:
    u32 mButton = 0;
    u32 mMods = 0;
};
REGISTER_CLASS(InputEventMouseButtonPressed);

class InputEventMouseButtonHold: public Event
{
public:
    u32 mButton = 0;
    u32 mMods = 0;
};
REGISTER_CLASS(InputEventMouseButtonHold);

class InputEventMouseButtonReleased: public Event
{
public:
    u32 mButton = 0;
    u32 mMods = 0;
};
REGISTER_CLASS(InputEventMouseButtonReleased);

class InputEventKeyPressed: public Event
{
public:
    u32 mKey = 0;
    u32 mMods = 0;
};
REGISTER_CLASS(InputEventKeyPressed);

class InputEventKeyHold: public Event
{
public:
    u32 mKey = 0;
    u32 mMods = 0;
};
REGISTER_CLASS(InputEventKeyHold);

class InputEventKeyReleased: public Event
{
public:
    u32 mKey = 0;
    u32 mMods = 0;
};
REGISTER_CLASS(InputEventKeyReleased);

class InputEventScroll: public Event
{
public:
    f32 mScroll = 0.0f;
};
REGISTER_CLASS(InputEventScroll);

class InputEventChar: public Event
{
public:
    char mChar;
};
REGISTER_CLASS(InputEventChar);
