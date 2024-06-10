#include "Graphics/Module.hpp"
#include "UI/UIUtils.hpp"

f32 UIUtils::correctAspectRatio(f32 x)
{
    const f32 aspectRation = GET_SYSTEM(WindowManager).getMainWindow()->getAspectRatio();
	const f32 corrected = x / aspectRation;;
	return corrected;
}
Vector3 UIUtils::correctAspectRatioVectorX(const Vector3& vector)
{
	Vector3 correctedVector = vector;
	correctedVector.x = correctAspectRatio(vector.x);
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
