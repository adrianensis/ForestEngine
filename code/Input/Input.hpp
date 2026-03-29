#pragma once

#include "Core/Core.hpp"
#include "Core/System/System.hpp"
#include "Input/InputEvents.hpp"

NS_BEGIN(Input)
class Input;
class IWindowInputAdapter
{
public:
    virtual ~IWindowInputAdapter() = default;
    virtual void keyCallback(int key, int scancode, int action, int mods) = 0;
    virtual void mouseButtonCallback(int button, int action, int mods) = 0;
    virtual void scrollCallback(double xoffset, double yoffset) = 0;
    virtual void charCallback(unsigned int codepoint) = 0;
    virtual Maths::Vector2 getMousePosition() const = 0;

    void setInput(Input* input) { mInput = input; }
protected:
    Input* mInput = nullptr;
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

    void setWindowInputAdapter(IWindowInputAdapter* windowInputAdapter);
    
public:
	Maths::Vector2 smMouseCoordinates;
	Core::i32 smLastMouseButtonPressed;
	Core::i32 smLastKeyPressed;
	Core::i32 smModifier;
	bool smKeyJustPressed;
	bool smButtonJustPressed;
	Core::f32 smScroll;

private:
    IWindowInputAdapter* mWindowInputAdapter = nullptr;

public:
    GET(WindowInputAdapter);
};
REGISTER_CLASS(Input, System);
NS_END