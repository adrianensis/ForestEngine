#pragma once

#include "Core/System/System.hpp"
#include "Input/InputEvents.hpp"

NS_BEGIN(Input)
class Input;

class InputCursorPosition
{
public:
    double x = 0;
    double y = 0;
};

class IWindowInputAdapter
{
public:
    virtual ~IWindowInputAdapter() = default;
    virtual void keyCallback(int key, int scancode, int action, int mods) = 0;
    virtual void mouseButtonCallback(int button, int action, int mods) = 0;
    virtual void scrollCallback(double xoffset, double yoffset) = 0;
    virtual void charCallback(unsigned int codepoint) = 0;
    virtual InputCursorPosition getMousePosition() const = 0;

    void setInput(Input* input) { mInput = input; }
protected:
    Input* mInput = nullptr;
};

class Input: public System::System, public Event::IEventObject
{
public:
    void init();
    void update();
    bool isKeyPressedOnce(int key);
    bool isKeyPressed(int key);
    bool isModifierPressed(int modifier);
    bool isMouseButtonPressedOnce(int button);
    bool isMouseButtonPressed(int button);
    const InputCursorPosition& getMousePosition();
    float getScroll();
    void clearMouseButton();
    void clearKey();

    void setWindowInputAdapter(IWindowInputAdapter* windowInputAdapter);
    
public:
	InputCursorPosition smMouseCoordinates;
	int smLastMouseButtonPressed;
	int smLastKeyPressed;
	int smModifier;
	bool smKeyJustPressed;
	bool smButtonJustPressed;
	float smScroll;

private:
    IWindowInputAdapter* mWindowInputAdapter = nullptr;

public:
    GET(WindowInputAdapter);
};
REGISTER_CLASS(Input, System);
NS_END