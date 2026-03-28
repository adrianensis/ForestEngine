#pragma once

#include "Graphics/Camera/Camera.hpp"

class CameraManager
{
private:
	Camera* mCamera = nullptr;

public:
    GET_SET(Camera)
};
REGISTER_CLASS(CameraManager, System);
