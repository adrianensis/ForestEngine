#pragma once

#include "Core/ECS/System.hpp"
#include "Graphics/Camera/Camera.hpp"

class CameraManager: public System
{
private:
	TComponentHandler<Camera> mCamera;

public:
    GET_SET(Camera)
};
REGISTER_CLASS(CameraManager);
