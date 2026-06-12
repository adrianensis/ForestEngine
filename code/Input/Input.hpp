#pragma once

namespace Input
{
class IInput;

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
    virtual void cursorPositionCallback(double x, double y) = 0;
    virtual InputCursorPosition getMousePosition() const = 0;

    void setInput(IInput* input) { mInput = input; }
protected:
    IInput* mInput = nullptr;
};

class IInput
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
    
protected:
    void onCursorPositionChanged();

public:
	InputCursorPosition mMouseCoordinates;
	int mLastMouseButtonPressed;
	int mLastKeyPressed;
	int mModifier;
	bool mKeyJustPressed;
	bool mButtonJustPressed;
	float mScroll;

protected:
    IWindowInputAdapter* mWindowInputAdapter = nullptr;

public:
    IWindowInputAdapter* getWindowInputAdapter() const { return mWindowInputAdapter; };
};
};