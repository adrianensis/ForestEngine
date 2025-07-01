#pragma once

#include "GPU/Camera/GPUFrustum.hpp"

class GPUCamera
{
public:

    void init();
    void update(const Matrix4& viewMatrix);
    void recalculateProjectionMatrix();
    void setOrtho(Core::f32 left, Core::f32 right, Core::f32 bottom, Core::f32 top, Core::f32 near, Core::f32 far);
    void setPerspective(Core::f32 near, Core::f32 far, Core::f32 aspect, Core::f32 fov);
	void onResize(const Vector2& windowSize, Core::f32 aspectRatio);
    Vector3 screenToWorld(const Vector2& screenPosition, Core::f32 depth);
    Vector2 worldToScreen(const Vector3& worldPosition);
    void setZoom(Core::f32 zoom);
    void zoomIn(Core::f32 zoomDelta);
    void zoomOut(Core::f32 zoomDelta);
    void resetZoom();

private:
    void calculateViewMatrix();
    void calculateProjectionViewMatrix();
    void calculateInverseMatrix(bool force = false);

private:
	Matrix4 mInverseProjectionMatrix;
	Matrix4 mInverseViewMatrix;
	Matrix4 mInversePVMatrix;

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
	
	Matrix4 mProjectionMatrix;
	mutable Matrix4 mViewMatrix;
	mutable Matrix4 mProjectionViewMatrix;
	bool mIsOrtho = false;
	GPUFrustum mFrustum;
	Core::f32 mZoom = 1.0f;
};