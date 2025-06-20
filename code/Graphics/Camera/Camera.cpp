
#include "Scene/Module.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Core/Window/WindowManager.hpp"

void Camera::init()
{
	mGPUCamera.init();
}

void Camera::onComponentAdded()
{
    mGPUCamera.recalculateProjectionMatrix();
}

void Camera::update()
{
	PROFILER_CPU()
	
	const Matrix4& viewMatrix = EntityComponentManager::getInstance().getFirstComponent<Transform>(getOwnerEntity())->getViewMatrix();
	mGPUCamera.update(viewMatrix);
}

void Camera::onResize()
{
	Vector2 windowSize = GET_SYSTEM(WindowManager).getMainWindow()->getWindowSize();
	f32 aspectRatio = GET_SYSTEM(WindowManager).getMainWindow()->getAspectRatio();
    mGPUCamera.onResize(windowSize, aspectRatio);
}