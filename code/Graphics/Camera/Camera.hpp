#pragma once

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Camera/Frustum.hpp"

class Camera: public Component
{
    GENERATE_METADATA(Camera)

private:
	Matrix4 mProjectionMatrix;
	mutable Matrix4 mViewMatrix;
	mutable Matrix4 mProjectionViewMatrix;
	Matrix4 mInversePVMatrix; // used in screen to world calculations.

	mutable bool mViewMatrixNeedsUpdate = true;
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
	
	bool mIsOrtho = false;

	Frustum mFrustum;
	f32 mZoom = 1.0f;

	TransformState mTransformState;

	void calculateInverseMatrix(bool force = false);

public:

	void init() override;
	void onComponentAdded() override;
	void recalculateProjectionMatrix();

	void update();

	const Matrix4& getViewMatrix() const;
	const Matrix4& getProjectionViewMatrix() const;

	void setOrtho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
	void setPerspective(f32 near, f32 far, f32 aspect, f32 fov);

	void onResize();

	Vector3 screenToWorld(const Vector2& screenPosition);

	void setZoom(f32 zoom);

	void zoomIn(f32 zoomDelta);
	void zoomOut(f32 zoomDelta);
	void resetZoom()
	{
		mZoom = 1;
		setZoom(mZoom);
	};

	CRGET(ProjectionMatrix)
	GET(IsOrtho)
	CRGET(Frustum)
	GET(Zoom)
};