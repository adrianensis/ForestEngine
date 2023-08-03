#pragma once

#include "Core/Module.hpp"

class IOcTreeElement
{
public:
	virtual Vector3 getOcTreeElementCenter() const { return Vector3(0,0,0);}
	virtual f32 getOcTreeElementRadius() const { return 0;}
};