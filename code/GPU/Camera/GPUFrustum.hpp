#pragma once

#include "Core/Minimal.hpp"

class GPUCamera;

class GPUFrustum
{
public:
    void init(GPUCamera *camera);
    bool testSphere(const Maths::Vector3& center, Core::f32 radius) const;
    bool testPoint(const Maths::Vector3& point) const;
    bool testRectangle(const Maths::Vector3& leftTop, Core::f32 width, Core::f32 height) const;
    void build();
	
private: 
	Maths::Matrix4 mVPmatrix;
	GPUCamera* mCamera = nullptr;
	std::array<Maths::Vector4, 6> mPlanes;
};