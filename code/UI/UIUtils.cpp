#include "Graphics/Module.hpp"
#include "UI/UIUtils.hpp"

Core::f32 UIUtils::correctAspectRatio(Core::f32 x)
{
    const Core::f32 aspectRation = GET_SYSTEM(Window::WindowManager).getMainWindow()->getAspectRatio();
	const Core::f32 corrected = x / aspectRation;;
	return corrected;
}
Maths::Vector3 UIUtils::correctAspectRatioVectorX(const Maths::Vector3& vector)
{
	Maths::Vector3 correctedVector = vector;
	correctedVector.x = correctAspectRatio(vector.x);
	return correctedVector;
}

Maths::Vector3 UIUtils::toScreenSpace(const Maths::Vector3& vector)
{
	Maths::Vector3 correctedVector = vector;
    Maths::Vector3 windowSize = GET_SYSTEM(Window::WindowManager).getMainWindow()->getWindowSize();
    windowSize.z = 1;
	correctedVector = correctedVector / windowSize;
	return correctedVector;
}
