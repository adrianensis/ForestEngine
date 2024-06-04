#include "Graphics/Module.hpp"
#include "UI/UIUtils.hpp"

Vector3 UIUtils::correctAspectRatio_X(const Vector3& vector)
{
	Vector3 correctedVector = vector;
    f32 aspectRation = GET_SYSTEM(WindowManager).getMainWindow()->getAspectRatio();
	correctedVector.x = correctedVector.x / aspectRation;
	return correctedVector;
}

Vector3 UIUtils::toScreenSpace(const Vector3& vector)
{
	Vector3 correctedVector = vector;
    Vector3 windowSize = GET_SYSTEM(WindowManager).getMainWindow()->getWindowSize();
    windowSize.z = 1;
	correctedVector = correctedVector / windowSize;
	return correctedVector;
}
