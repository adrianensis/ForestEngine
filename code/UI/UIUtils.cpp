#include "Graphics/Module.hpp"
#include "UI/UIUtils.hpp"


Vector2 UIUtils::correctAspectRatio_X(const Vector2& vector)
{
	Vector2 correctedVector = vector;
	correctedVector.x = correctedVector.x / GET_SYSTEM(Window).getAspectRatio();
	return correctedVector;
}
