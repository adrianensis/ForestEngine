#pragma once

#include "GPU/Camera/GPUFrustum.hpp"

class GPUCamera
{
public:

    void init();
    void update(const Maths::Matrix4& viewMatrix);
    void recalculateProjectionMatrix();
    void setOrtho(GPU::f32 left, GPU::f32 right, GPU::f32 bottom, GPU::f32 top, GPU::f32 near, GPU::f32 far);
    void setPerspective(GPU::f32 near, GPU::f32 far, GPU::f32 aspect, GPU::f32 fov);
	void onResize(const Maths::Vector2& windowSize, GPU::f32 aspectRatio);
    Maths::Vector3 screenToWorld(const Maths::Vector2& screenPosition, GPU::f32 depth);
    Maths::Vector2 worldToScreen(const Maths::Vector3& worldPosition);
    void setZoom(GPU::f32 zoom);
    void zoomIn(GPU::f32 zoomDelta);
    void zoomOut(GPU::f32 zoomDelta);
    void resetZoom();

private:
    void calculateViewMatrix();
    void calculateProjectionViewMatrix();
    void calculateInverseMatrix(bool force = false);

private:
	Maths::Matrix4 mInverseProjectionMatrix;
	Maths::Matrix4 mInverseViewMatrix;
	Maths::Matrix4 mInversePVMatrix;

	bool mInversePVMatrixNeedsUpdate = true;
	
public:
	GPU::f32 mLeft = 0.0f;
	GPU::f32 mRight = 0.0f;
	GPU::f32 mBottom = 0.0f;
	GPU::f32 mTop = 0.0f;
	GPU::f32 mNear = 0.0f;
	GPU::f32 mFar = 0.0f;
	
	GPU::f32 mAspect = 0.0f;
	GPU::f32 mFov = 0.0f;
	
	Maths::Matrix4 mProjectionMatrix;
	mutable Maths::Matrix4 mViewMatrix;
	mutable Maths::Matrix4 mProjectionViewMatrix;
	bool mIsOrtho = false;
	GPUFrustum mFrustum;
	GPU::f32 mZoom = 1.0f;
};