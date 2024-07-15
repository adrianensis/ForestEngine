#pragma once

#include "Core/System/System.hpp"
#include "Graphics/Camera/Camera.hpp"

class CameraManager: public System
{
private:
	Ptr<Camera> mCamera;

public:
    GET_SET(Camera)
};
REGISTER_CLASS(CameraManager);
