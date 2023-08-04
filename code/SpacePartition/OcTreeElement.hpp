#pragma once

#include "Core/Module.hpp"

class IOcTreeElement
{
public:
    void init(const Matrix4& modelMatrix, const Vector3& meshMin, const Vector3& meshMax);
	Vector3 getOcTreeElementCenter() const { return mOcTreeBoundingBox.getCenter();}
	f32 getOcTreeElementRadius() const { return mOcTreeBoundingBox.getSize().max();}
	virtual bool isOcTreeElementStatic() const { return false;}

private:
    Cube mOcTreeBoundingBox;

public:
    CRGET(OcTreeBoundingBox)
};