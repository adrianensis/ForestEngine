
#include "Scene/Module.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/RenderEngine.hpp"


void Camera::init()
{
	TRACE()

	mIsOrtho = true;
	mZoom = 1;
	
	mViewMatrix.identity();
	mInversePVMatrix.identity();
	mFrustum.init(this);
}

void Camera::onComponentAdded()
{
	recalculateProjectionMatrix();

	mTransformState = TransformState(getGameObject()->getTransform().get());
}

void Camera::update()
{
	PROFILER_CPU()
	
	TransformState currentTransformState(getGameObject()->getTransform().get());
	if(!currentTransformState.eq(mTransformState))
	{
		mFrustum.build();

		mInversePVMatrixNeedsUpdate = true;

		calculateProjectionViewMatrix();

		mTransformState = currentTransformState;
	}

}

void Camera::recalculateProjectionMatrix()
{
	PROFILER_CPU()

	if (mIsOrtho)
	{
		setOrtho(mLeft, mRight, mBottom, mTop, mNear, mFar);
	}
	else
	{
		setPerspective(mNear, mFar, mAspect, mFov);
	}

	calculateInverseMatrix(true);
	mFrustum.build();
}

void Camera::setOrtho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
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
}

void Camera::setPerspective(f32 near, f32 far, f32 aspect, f32 fov)
{
	mIsOrtho = false;

	mNear = near;
	mFar = far;
	mAspect = aspect;
	mFov = fov;
	
	mProjectionMatrix.perspective(mNear, mFar, mAspect, mFov * mZoom);
}

void Camera::onResize()
{
	recalculateProjectionMatrix();
}

const Matrix4& Camera::getViewMatrix() const
{


	return mViewMatrix;
}

const Matrix4& Camera::getProjectionViewMatrix() const
{


	return mProjectionViewMatrix;
}

Vector3 Camera::screenToWorld(const Vector2& screenPosition)
{	
	calculateInverseMatrix();
	Vector4 v = mInversePVMatrix.mulVector(Vector4(screenPosition.x, screenPosition.y, 0, 1.0));

	v.x = v.x / v.w;
	v.y = v.y / v.w;
	v.z = v.z / v.w;

	return v;
}

void Camera::setZoom(f32 zoom)
{
	mZoom = zoom;
	recalculateProjectionMatrix();
}

void Camera::zoomIn(f32 zoomDelta)
{
	f32 newZoom = mZoom - zoomDelta;

	if(newZoom < 0)
	{
		newZoom = 0;
	}

	setZoom(newZoom);
}

void Camera::zoomOut(f32 zoomDelta)
{
	f32 newZoom = mZoom + zoomDelta;
	setZoom(newZoom);
}

void Camera::resetZoom()
{
	mZoom = 1;
	setZoom(mZoom);
}

void Camera::calculateViewMatrix()
{
	PROFILER_CPU()

	Vector3 originalPosition = getGameObject()->getTransform().get().getWorldPosition();

	Matrix4 viewTranslationMatrix;
	viewTranslationMatrix.translation(-originalPosition);

	Vector3 worldUp(0,1,0);

	Vector3 forward(0,0,1);
	forward = forward * -1;

	Vector3 right = Vector3(forward).cross(worldUp).nor();

	Vector3 up = Vector3(right).cross(forward).nor();

	Matrix4 viewRotationMatrix;
	viewRotationMatrix.identity();

	viewRotationMatrix.init(
		Vector4(right.x, right.y, right.z, 0),
		Vector4(up.x, up.y, up.z, 0),
		Vector4(forward.x, forward.y, forward.z, 0),
		Vector4(0, 0, 0, 1)
	);
	
	viewRotationMatrix.transpose();

	Matrix4 rotationMatrix = getGameObject()->getTransform().get().getRotationMatrix();

	mViewMatrix.init(viewRotationMatrix);
	mViewMatrix.mul(rotationMatrix);
	mViewMatrix.mul(viewTranslationMatrix);
}

void Camera::calculateProjectionViewMatrix()
{
	calculateViewMatrix();
	mProjectionViewMatrix.init(getProjectionMatrix());
	mProjectionViewMatrix.mul(getViewMatrix());
}

void Camera::calculateInverseMatrix(bool force /*= false*/)
{
	if(mInversePVMatrixNeedsUpdate || force)
	{
		calculateProjectionViewMatrix();

		Matrix4 inverseProjectionMatrix;
		mInversePVMatrix.init(getProjectionViewMatrix());		
		mInversePVMatrix.invert();

		mInversePVMatrixNeedsUpdate = false;
	}
}
