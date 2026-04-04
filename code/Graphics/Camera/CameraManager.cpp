#include "Graphics/Camera/CameraManager.hpp"

void CameraManager::update()
{
    if(mCamera)
    {
        mCamera->update();
    }
}