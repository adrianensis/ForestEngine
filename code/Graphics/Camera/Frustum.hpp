#pragma once

#include "Core/Module.hpp"

class Camera;

class Frustum
{
public:
    void init(Camera *camera);
    bool testSphere(const Vector3& center, f32 radius) const;
    bool testPoint(const Vector3& point) const;
    bool testRectangle(const Vector3& leftTop, f32 width, f32 height) const;
    void build();
	
private: 
	Matrix4 mVPmatrix;
	Camera* mCamera = nullptr;
	std::array<Vector4, 6> mPlanes;
};
