#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Module.hpp"
#include "UI/UIUtils.hpp"
#endif

class UIUtils
{
public:

	CPP static Vector2 correctAspectRatio_X(const Vector2& vector)
	{
		Vector2 correctedVector = vector;
		correctedVector.x = correctedVector.x / RenderContext::getAspectRatio();
		return correctedVector;
	}
};
