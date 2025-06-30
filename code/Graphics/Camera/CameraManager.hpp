#pragma once

#include "Core/System/System.hpp"
#include "Graphics/Camera/Camera.hpp"

class CameraManager: public Core::System
{
private:
	TComponentPtr<Camera> mCamera;

public:
    GET_SET(Camera)
};
REGISTER_CLASS(CameraManager);
