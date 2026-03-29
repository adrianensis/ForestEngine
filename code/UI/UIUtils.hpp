#pragma once

#include "Engine/Core.hpp"
#include "Window/Window.hpp"

class UIUtils
{
public:

    static Core::f32 correctAspectRatio(Window::Window* window, Core::f32 x);
    static Maths::Vector3 correctAspectRatioVectorX(Window::Window* window, const Maths::Vector3& vector);
    static Maths::Vector3 toScreenSpace(Window::Window* window, const Maths::Vector3& vector);
};

