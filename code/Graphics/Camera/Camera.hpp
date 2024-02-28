#pragma once

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Camera/Frustum.hpp"

class Camera: public Component
{
public:

    void init();
    void onComponentAdded();
    void update();
    void recalculateProjectionMatrix();
    void setOrtho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
    void setPerspective(f32 near, f32 far, f32 aspect, f32 fov);
    void onResize();
    Vector3 screenToWorld(const Vector2& screenPosition);
    void setZoom(f32 zoom);
    void zoomIn(f32 zoomDelta);
    void zoomOut(f32 zoomDelta);
    void resetZoom();

private:
    void calculateViewMatrix();
    void calculateProjectionViewMatrix();
    void calculateInverseMatrix(bool force = false);

private:
	Matrix4 mInversePVMatrix; // used in screen to world calculations.

	mutable bool mProjectionViewMatrixNeedsUpdate = true;
	mutable bool mInversePVMatrixNeedsUpdate = true;

	f32 mLeft = 0.0f;
	f32 mRight = 0.0f;
	f32 mBottom = 0.0f;
	f32 mTop = 0.0f;
	f32 mNear = 0.0f;
	f32 mFar = 0.0f;

	f32 mAspect = 0.0f;
	f32 mFov = 0.0f;

public:
	Matrix4 mProjectionMatrix;
	mutable Matrix4 mViewMatrix;
	mutable Matrix4 mProjectionViewMatrix;
	bool mIsOrtho = false;
	Frustum mFrustum;
	f32 mZoom = 1.0f;
};
REGISTER_CLASS(Camera);
