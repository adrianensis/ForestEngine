#include "Graphics/Module.hpp"
#include "UI/UIUtils.hpp"

Vector2 UIUtils::correctAspectRatio_X(const Vector2& vector)
{
	Vector2 correctedVector = vector;
    f32 aspectRation = GET_SYSTEM(WindowManager).getMainWindow()->getAspectRatio();
	correctedVector.x = correctedVector.x / aspectRation;
	return correctedVector;
}

Vector2 UIUtils::toScreenSpace(const Vector2& vector)
{
	Vector2 correctedVector = vector;
    Vector2 windowSize = GET_SYSTEM(WindowManager).getMainWindow()->getWindowSize();
	correctedVector = correctedVector / windowSize;
	return correctedVector;
}
