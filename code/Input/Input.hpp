#pragma once

#include "Core/Core.hpp"
#include "Core/System/System.hpp"
#include "Input/InputEvents.hpp"

#define SEND_INPUT_EVENT(event) SEND_EVENT(nullptr, &GET_SYSTEM(::Input::Input), event);

NS_BEGIN(Input)
class IWindowInputAdapter
{
public:
    virtual ~IWindowInputAdapter() = default;
    virtual void keyCallback(int key, int scancode, int action, int mods) = 0;
    virtual void mouseButtonCallback(int button, int action, int mods) = 0;
    virtual void scrollCallback(double xoffset, double yoffset) = 0;
    virtual void charCallback(unsigned int codepoint) = 0;
    virtual Maths::Vector2 getMousePosition() const = 0;
};

class Input: public System::System, public Event::IEventObject
{
public:
    void init();
    void update();
    bool isKeyPressedOnce(Core::i32 key);
    bool isKeyPressed(Core::i32 key);
    bool isModifierPressed(Core::i32 modifier);
    bool isMouseButtonPressedOnce(Core::i32 button);
    bool isMouseButtonPressed(Core::i32 button);
    const Maths::Vector2& getMousePosition();
    Core::f32 getScroll();
    void clearMouseButton();
    void clearKey();
    
public:
	Maths::Vector2 smMouseCoordinates;
	Core::i32 smLastMouseButtonPressed;
	Core::i32 smLastKeyPressed;
	Core::i32 smModifier;
	bool smKeyJustPressed;
	bool smButtonJustPressed;
	Core::f32 smScroll;

private:
    Core::WeakPtr<IWindowInputAdapter> mWindowInputAdapter;

public:
    GET_SET(WindowInputAdapter);
};
REGISTER_CLASS(Input, System);
NS_END