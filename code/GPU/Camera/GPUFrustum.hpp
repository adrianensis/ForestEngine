#pragma once

#include "Core/Minimal.hpp"

class GPUCamera;

class GPUFrustum
{
public:
    void init(GPUCamera *camera);
    bool testSphere(const Vector3& center, Core::f32 radius) const;
    bool testPoint(const Vector3& point) const;
    bool testRectangle(const Vector3& leftTop, Core::f32 width, Core::f32 height) const;
    void build();
	
private: 
	Matrix4 mVPmatrix;
	GPUCamera* mCamera = nullptr;
	std::array<Vector4, 6> mPlanes;
};