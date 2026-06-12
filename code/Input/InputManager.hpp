#pragma once

#include "Core/System/System.hpp"
#include "Input/Input.hpp"

#include "Input/InputEvents.hpp"

NS_BEGIN(Input)
class InputManager : public System::System
{
public:
    void init(IInput* input) { mInput = input; mInput->init(); };
    void terminate() { /*mInput.terminate();*/ };

private:
	IInput* mInput = nullptr;
public:
    IInput* getInput() const { return mInput; }
};
REGISTER_CLASS(InputManager, System);
NS_END