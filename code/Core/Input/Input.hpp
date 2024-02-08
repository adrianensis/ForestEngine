#pragma once

#include "Core/Object/ObjectBase.hpp"
#include "Core/Systems/System.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Input/InputEvents.hpp"

class IWindowInputAdapter
{
public:
    virtual void keyCallback(int key, int scancode, int action, int mods) = 0;
    virtual void mouseButtonCallback(int button, int action, int mods) = 0;
    virtual void scrollCallback(double xoffset, double yoffset) = 0;
    virtual void charCallback(unsigned int codepoint) = 0;
    virtual Vector2 getMousePosition() const = 0;
};

class Input: public System
{
	GENERATE_METADATA(Input)
public:
    void init(const Ptr<IWindowInputAdapter>& windowInputAdapter);
    void update();
    bool isKeyPressedOnce(i32 key);
    bool isKeyPressed(i32 key);
    bool isModifierPressed(i32 modifier);
    bool isMouseButtonPressedOnce(i32 button);
    bool isMouseButtonPressed(i32 button);
    const Vector2& getMousePosition();
    f32 getScroll();
    void clearMouseButton();
    void clearKey();
    
public:
	Vector2 smMouseCoordinates;
	i32 smLastMouseButtonPressed;
	i32 smLastKeyPressed;
	i32 smModifier;
	bool smKeyJustPressed;
	bool smButtonJustPressed;
	f32 smScroll;

private:
    Ptr<IWindowInputAdapter> mWindowInputAdapter;

public:
    GET(WindowInputAdapter);
};
