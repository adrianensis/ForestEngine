#pragma once

#include "Core/Module.hpp"

class IOcTreeElement
{
public:
    void init(const Matrix4& modelMatrix, const Vector3& meshMin, const Vector3& meshMax);
	virtual bool isOcTreeElementStatic() const { return false;}
	virtual bool isOcTreeTransformChanged() const { return false;}

private:
    Cube mOcTreeBoundingBox;

public:
    CRGET(OcTreeBoundingBox)
};