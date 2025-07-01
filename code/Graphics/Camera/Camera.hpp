#pragma once

#include "Engine/EntityComponent/Component.hpp"
#include "Scene/Transform.hpp"
#include "GPU/Camera/GPUCamera.hpp"

class Camera: public EC::Component
{
public:
    void init();
    void onComponentAdded();
    void update();
	void onResize();
private:
	GPUCamera mGPUCamera;
public:
	RGET(GPUCamera);
};
REGISTER_CLASS(Camera);
