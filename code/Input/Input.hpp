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
	int mLastMouseButtonPressed = -1;
	int mLastKeyPressed = -1;
	int mModifier = -1;
	bool mKeyJustPressed = false;
	bool mButtonJustPressed = false;
	float mScroll = 0;

protected:
    IWindowInputAdapter* mWindowInputAdapter = nullptr;

public:
    IWindowInputAdapter* getWindowInputAdapter() const { return mWindowInputAdapter; };
};
};