#include "Graphics/Module.hpp"
#include "UI/UIUtils.hpp"


Vector2 UIUtils::correctAspectRatio_X(CR(Vector2) vector)
{
	Vector2 correctedVector = vector;
	correctedVector.x = correctedVector.x / RenderContext::getAspectRatio();
	return correctedVector;
}
