#pragma once

#include "Scene/GameComponent.hpp"
#include "Scene/Transform.hpp"
#include "GPU/Camera/GPUCamera.hpp"

class Camera: public GameComponent
{
public:
    void init();
    void onECComponentAdded() override;
    void update();
	void onResize();
private:
	GPUCamera mGPUCamera;
public:
	RGET(GPUCamera);
};
REGISTER_CLASS(Camera);
