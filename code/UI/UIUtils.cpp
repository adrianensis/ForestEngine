
#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "UI/UIUtils.hpp"

Core::f32 UIUtils::correctAspectRatio(Window::IWindow* window, Core::f32 x)
{
    const Core::f32 aspectRation = window->getAspectRatio();
	const Core::f32 corrected = x / aspectRation;;
	return corrected;
}
Maths::Vector3 UIUtils::correctAspectRatioVectorX(Window::IWindow* window, const Maths::Vector3& vector)
{
	Maths::Vector3 correctedVector = vector;
	correctedVector.x = correctAspectRatio(window, vector.x);
	return correctedVector;
}

Maths::Vector3 UIUtils::toScreenSpace(Window::IWindow* window, const Maths::Vector3& vector)
{
	Maths::Vector3 correctedVector = vector;
    Maths::Vector3 windowSize(window->getWindowSize().x, window->getWindowSize().y, 1);
	correctedVector = correctedVector / windowSize;
	return correctedVector;
}
