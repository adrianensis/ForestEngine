
#include "Scene/Module.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Window/Window.hpp"

void Camera::init()
{
	LOG_TRACE()

	mIsOrtho = true;
	mZoom = 1;
	
	mViewMatrix.identity();
	mInversePVMatrix.identity();
	mFrustum.init(this);
}

void Camera::onComponentAdded()
{
    recalculateProjectionMatrix();
}

void Camera::update()
{
	PROFILER_CPU()
	
	mFrustum.build();
    mInversePVMatrixNeedsUpdate = true;
    calculateProjectionViewMatrix();
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

	mProjectionMatrix.ortho(mLeft * mZoom, mRight * mZoom, mBottom* mZoom,
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
    // if (mIsOrtho)
	// {
    //     Vector2 windowSize = GET_SYSTEM(Window).getWindowSize();
    //     setOrtho(-windowSize.x, windowSize.x, -windowSize.y, windowSize.y, mNear, mFar);
	// }
	// else
	// {
	// 	setPerspective(mNear, mFar, GET_SYSTEM(Window).getAspectRatio(), mFov);
	// }

	recalculateProjectionMatrix();
}

Vector3 Camera::screenToWorld(const Vector3& screenPosition)
{	
	calculateInverseMatrix();
    Matrix4 inverse;
    inverse.init(mProjectionMatrix);		
    inverse.invert();
	Vector4 v = inverse.mulVector(Vector4(screenPosition.x, screenPosition.y, screenPosition.z, 1.0));

    Vector3 result = v;
	result = result / v.w;

	return result;
}

Vector2 Camera::worldToScreen(const Vector3& worldPosition)
{	
    calculateProjectionViewMatrix();
	Vector4 v = mProjectionViewMatrix.mulVector(Vector4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0));

    Vector2 result = v;
	result = result / v.w;

	return result;
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

	Vector3 originalPosition = mGameObject->mTransform->getWorldPosition();

	Matrix4 viewTranslationMatrix;
	viewTranslationMatrix.translation(-originalPosition);
	Matrix4 rotationMatrix = mGameObject->mTransform->getLocalRotationMatrix();

	mViewMatrix.identity();
	mViewMatrix.mul(rotationMatrix);
	mViewMatrix.mul(viewTranslationMatrix);
}

void Camera::calculateProjectionViewMatrix()
{
	calculateViewMatrix();
	mProjectionViewMatrix.init(mProjectionMatrix);
	mProjectionViewMatrix.mul(mViewMatrix);
}

void Camera::calculateInverseMatrix(bool force /*= false*/)
{
	if(mInversePVMatrixNeedsUpdate || force)
	{
		calculateProjectionViewMatrix();

		mInversePVMatrix.init(mProjectionViewMatrix);		
		mInversePVMatrix.invert();

		mInversePVMatrixNeedsUpdate = false;
	}
}
