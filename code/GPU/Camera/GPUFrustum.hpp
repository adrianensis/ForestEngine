#pragma once

#include "GPU/Core/GPUDefinitions.h"
#include "Core/Core.hpp"

class GPUCamera;

class GPUFrustum
{
public:
    void init(GPUCamera *camera);
    bool testSphere(const Maths::Vector3& center, GPU::f32 radius) const;
    bool testPoint(const Maths::Vector3& point) const;
    bool testRectangle(const Maths::Vector3& leftTop, GPU::f32 width, GPU::f32 height) const;
    void build();
	
private: 
	Maths::Matrix4 mVPmatrix;
	GPUCamera* mCamera = nullptr;
	std::array<Maths::Vector4, 6> mPlanes;
};