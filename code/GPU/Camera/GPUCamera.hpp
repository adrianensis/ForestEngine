#pragma once

#include "GPU/Camera/GPUFrustum.hpp"

class GPUCamera
{
public:

    void init();
    void update(const Matrix4& viewMatrix);
    void recalculateProjectionMatrix();
    void setOrtho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
    void setPerspective(f32 near, f32 far, f32 aspect, f32 fov);
	void onResize(const Vector2& windowSize, f32 aspectRatio);
    Vector3 screenToWorld(const Vector2& screenPosition, f32 depth);
    Vector2 worldToScreen(const Vector3& worldPosition);
    void setZoom(f32 zoom);
    void zoomIn(f32 zoomDelta);
    void zoomOut(f32 zoomDelta);
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
	f32 mLeft = 0.0f;
	f32 mRight = 0.0f;
	f32 mBottom = 0.0f;
	f32 mTop = 0.0f;
	f32 mNear = 0.0f;
	f32 mFar = 0.0f;
	
	f32 mAspect = 0.0f;
	f32 mFov = 0.0f;
	
	Matrix4 mProjectionMatrix;
	mutable Matrix4 mViewMatrix;
	mutable Matrix4 mProjectionViewMatrix;
	bool mIsOrtho = false;
	GPUFrustum mFrustum;
	f32 mZoom = 1.0f;
};