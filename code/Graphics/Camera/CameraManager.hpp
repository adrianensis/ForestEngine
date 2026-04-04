#pragma once

#include "Graphics/Camera/Camera.hpp"

class CameraManager: public System::System
{
public:
    void update();
private:
	Camera* mCamera = nullptr;

public:
    GET_SET(Camera)
};
REGISTER_CLASS(CameraManager, System);
