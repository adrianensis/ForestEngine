#pragma once

#include "Core/ObjectBase.hpp"
#include "Core/EngineSystem.hpp"

#include "Graphics/RenderContext.hpp"
#include "Core/Maths/Vector2.hpp"

#include "Core/Input/InputEvents.hpp"

class Input: public EngineSystem
{
	GENERATE_METADATA(Input)
public:
    void init();
    void pollEvents();
    bool isKeyPressedOnce(i32 key);
    bool isKeyPressed(i32 key);
    bool isModifierPressed(i32 modifier);
    bool isMouseButtonPressedOnce(i32 button);
    bool isMouseButtonPressed(i32 button);
    const Vector2& getMousePosition();
    f32 getScroll();
    void clearMouseButton();
    void clearKey();
    void setCursorVisibility(bool visible);

private:
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    static void charCallback(GLFWwindow *window, unsigned int codepoint);

public:
	Vector2 smMouseCoordinates;
	i32 smLastMouseButtonPressed;
	i32 smLastKeyPressed;
	i32 smModifier;
	bool smKeyJustPressed;
	bool smButtonJustPressed;
	f32 smScroll;
};
