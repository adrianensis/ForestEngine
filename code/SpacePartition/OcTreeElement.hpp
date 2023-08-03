#pragma once

#include "Core/Module.hpp"

class IOcTreeElement
{
public:
    void init(const Matrix4& modelMatrix, const Vector3& meshMin, const Vector3& meshMax);
	virtual Vector3 getOcTreeElementCenter() const { return mOcTreeBoundingBox.getCenter();}
	virtual f32 getOcTreeElementRadius() const { return mOcTreeBoundingBox.getSize().max();}

private:
    Cube mOcTreeBoundingBox;

public:
    CRGET(OcTreeBoundingBox)
};