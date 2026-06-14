#pragma once

#include "Scene/GameComponent.hpp"
#include "Scene/Transform.hpp"
#include "GPU/Camera/GPUCamera.hpp"
#include "Window/Window.hpp"

class Camera: public GameComponent
{
public:
    void init();
    void onECComponentAdded() override;
    void update();
	void onResize(Window::IWindow* window);
private:
	GPUCamera mGPUCamera;
public:
	RGET(GPUCamera);
};
REGISTER_CLASS(Camera);
