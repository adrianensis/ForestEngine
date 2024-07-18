#pragma once

#include "Core/ECS/System.hpp"
#include "Graphics/Camera/Camera.hpp"

class CameraManager: public System
{
private:
	TypedComponentHandler<Camera> mCamera;

public:
    GET_SET(Camera)
};
REGISTER_CLASS(CameraManager);
