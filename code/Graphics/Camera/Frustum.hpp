#pragma once

#include "Core/Module.hpp"

class Camera;

class Frustum: public ObjectBase
{
    GENERATE_METADATA(Frustum)
	PRI Matrix4 mVPmatrix;
	PRI Camera* mCamera = nullptr;
	PRI std::array<Vector4, 6> mPlanes;

public:
	void init(Camera * camera);

	/**
	* Builds the frustum planes.
	*/
	void build();

	bool testSphere(const Vector3& center, f32 radius) const;
	bool testPoint(const Vector3& point) const;
	bool testRectangle(const Vector3& leftTop, f32 width, f32 height) const;
};