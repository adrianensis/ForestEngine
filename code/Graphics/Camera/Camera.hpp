#pragma once

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Camera/Frustum.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Camera/Camera.hpp"

#include "Scene/Module.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/RenderEngine.hpp"
#endif

class Camera: public Component
{
    GENERATE_METADATA(Camera)

public:

	CPP void init()
	{
		TRACE()

		mIsOrtho = true;
		mZoom = 1;
		
		mViewMatrix.identity();
		mInversePVMatrix.identity();
		mFrustum.init(this);
	}

	CPP void onComponentAdded()
	{
		recalculateProjectionMatrix();

		mTransformState = TransformState(getGameObject()->getTransform().get());
	}

	CPP void update()
	{
		PROFILER_TIMEMARK_START()

		TransformState currentTransformState(getGameObject()->getTransform().get());
		if(!currentTransformState.eq(mTransformState))
		{
			mFrustum.build();

			mInversePVMatrixNeedsUpdate = true;
			mViewMatrixNeedsUpdate = true;
			mProjectionViewMatrixNeedsUpdate = true;

			mTransformState = currentTransformState;
		}

		PROFILER_TIMEMARK_END()
	}

	CPP void recalculateProjectionMatrix()
	{
		if (mIsOrtho)
		{
			setOrtho(mLeft, mRight, mBottom, mTop, mNear, mFar);
		}
		else
		{
			setPerspective(mNear, mFar, mAspect, mFov);
		}

		mProjectionViewMatrixNeedsUpdate = true;
		calculateInverseMatrix(true);
		mFrustum.build();
	}

	CPP void setOrtho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
	{
		mIsOrtho = true;
		
		mLeft = left;
		mRight = right;
		mBottom = bottom;
		mTop = top;
		mNear = near;
		mFar = far;

		mProjectionMatrix.ortho(mLeft * RenderContext::getAspectRatio() * mZoom, mRight * RenderContext::getAspectRatio() * mZoom, mBottom* mZoom,
								mTop* mZoom, mNear, mFar);
	};

	CPP void setPerspective(f32 near, f32 far, f32 aspect, f32 fov)
	{
		mIsOrtho = false;

		mNear = near;
		mFar = far;
		mAspect = aspect;
		mFov = fov;
		
		mProjectionMatrix.perspective(mNear, mFar, mAspect, mFov * mZoom);
	};

	CPP void onResize()
	{
		recalculateProjectionMatrix();
	}

	CPP const Matrix4& getViewMatrix() const
	{
		if(mViewMatrixNeedsUpdate)
		{
			const Vector3& position = getGameObject()->getTransform().get().getWorldPosition();
			mViewMatrix.translation(position * -1);
			mViewMatrix.mul(getGameObject()->getTransform().get().getRotationMatrix());

			mViewMatrixNeedsUpdate = false;
		}

		return mViewMatrix;
	};

	CPP const Matrix4& getProjectionViewMatrix() const
	{
		if(mProjectionViewMatrixNeedsUpdate)
		{
			mProjectionViewMatrix.init(getProjectionMatrix());
			mProjectionViewMatrix.mul(getViewMatrix());
			mProjectionViewMatrixNeedsUpdate = false;
		}

		return mProjectionViewMatrix;
	}

	CPP Vector3 screenToWorld(const Vector2& screenPosition)
	{	
		calculateInverseMatrix();
		Vector4 v = mInversePVMatrix.mulVector(Vector4(screenPosition.x, screenPosition.y, 0, 1.0));

		v.x = v.x / v.w;
		v.y = v.y / v.w;
		v.z = v.z / v.w;

		return v;
	}

	CPP void setZoom(f32 zoom)
	{
		mZoom = zoom;
		recalculateProjectionMatrix();
	}

	CPP void zoomIn(f32 zoomDelta)
	{
		f32 newZoom = mZoom - zoomDelta;

		if(newZoom < 0)
		{
			newZoom = 0;
		}

		setZoom(newZoom);
	}

	CPP void zoomOut(f32 zoomDelta)
	{
		f32 newZoom = mZoom + zoomDelta;
		setZoom(newZoom);
	}

	CPP void resetZoom()
	{
		mZoom = 1;
		setZoom(mZoom);
	};

private:
	CPP void calculateInverseMatrix(bool force = false)
	{
		if(mInversePVMatrixNeedsUpdate || force)
		{
			Matrix4 inverseProjectionMatrix;
			mInversePVMatrix.init(getProjectionViewMatrix());		
			mInversePVMatrix.invert();

			mInversePVMatrixNeedsUpdate = false;
		}
	}

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

public:
	CRGET(ProjectionMatrix)
	GET(IsOrtho)
	CRGET(Frustum)
	GET(Zoom)
};