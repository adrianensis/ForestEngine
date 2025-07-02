#pragma once

#include "GPU/Camera/GPUFrustum.hpp"

class GPUCamera
{
public:

    void init();
    void update(const Maths::Matrix4& viewMatrix);
    void recalculateProjectionMatrix();
    void setOrtho(Core::f32 left, Core::f32 right, Core::f32 bottom, Core::f32 top, Core::f32 near, Core::f32 far);
    void setPerspective(Core::f32 near, Core::f32 far, Core::f32 aspect, Core::f32 fov);
	void onResize(const Maths::Vector2& windowSize, Core::f32 aspectRatio);
    Maths::Vector3 screenToWorld(const Maths::Vector2& screenPosition, Core::f32 depth);
    Maths::Vector2 worldToScreen(const Maths::Vector3& worldPosition);
    void setZoom(Core::f32 zoom);
    void zoomIn(Core::f32 zoomDelta);
    void zoomOut(Core::f32 zoomDelta);
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
	Core::f32 mLeft = 0.0f;
	Core::f32 mRight = 0.0f;
	Core::f32 mBottom = 0.0f;
	Core::f32 mTop = 0.0f;
	Core::f32 mNear = 0.0f;
	Core::f32 mFar = 0.0f;
	
	Core::f32 mAspect = 0.0f;
	Core::f32 mFov = 0.0f;
	
	Maths::Matrix4 mProjectionMatrix;
	mutable Maths::Matrix4 mViewMatrix;
	mutable Maths::Matrix4 mProjectionViewMatrix;
	bool mIsOrtho = false;
	GPUFrustum mFrustum;
	Core::f32 mZoom = 1.0f;
};