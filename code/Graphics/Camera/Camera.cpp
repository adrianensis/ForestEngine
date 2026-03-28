
#include "Scene/Module.hpp"
#include "Graphics/Camera/Camera.hpp"

void Camera::init()
{
	mGPUCamera.init();
}

void Camera::onECComponentAdded()
{
    mGPUCamera.recalculateProjectionMatrix();
}

void Camera::update()
{
	PROFILER_CPU()
	
	const Maths::Matrix4& viewMatrix = ECManager.getFirstComponent<Transform>(getOwnerEntity())->getViewMatrix();
	mGPUCamera.update(viewMatrix);
}

void Camera::onResize(Core::WeakPtr<Window::Window> window)
{
	Maths::Vector2 windowSize = window->getWindowSize();
	Core::f32 aspectRatio = window->getAspectRatio();
    mGPUCamera.onResize(windowSize, aspectRatio);
}