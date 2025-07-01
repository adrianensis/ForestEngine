
#include "Scene/Module.hpp"
#include "GPU/Camera/GPUCamera.hpp"

void GPUCamera::init()
{
	mIsOrtho = true;
	mZoom = 1;
	
	mViewMatrix.identity();
	mInversePVMatrix.identity();
	mFrustum.init(this);
}

void GPUCamera::update(const Matrix4& viewMatrix)
{
	PROFILER_CPU()
	
	mViewMatrix = viewMatrix;
	mFrustum.build();
    mInversePVMatrixNeedsUpdate = true;
    calculateProjectionViewMatrix();
}

void GPUCamera::recalculateProjectionMatrix()
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

void GPUCamera::setOrtho(Core::f32 left, Core::f32 right, Core::f32 bottom, Core::f32 top, Core::f32 near, Core::f32 far)
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

void GPUCamera::setPerspective(Core::f32 near, Core::f32 far, Core::f32 aspect, Core::f32 fov)
{
	mIsOrtho = false;

	mNear = near;
	mFar = far;
	mAspect = aspect;
	mFov = fov;
	
	mProjectionMatrix.perspective(mNear, mFar, mAspect, mFov * mZoom);
}

void GPUCamera::onResize(const Vector2& windowSize, Core::f32 aspectRatio)
{
    if (mIsOrtho)
	{
        setOrtho(-windowSize.x, windowSize.x, -windowSize.y, windowSize.y, mNear, mFar);
	}
	else
	{
		setPerspective(mNear, mFar, aspectRatio, mFov);
	}

	recalculateProjectionMatrix();
}

Vector3 GPUCamera::screenToWorld(const Vector2& screenPosition, Core::f32 depth)
{	
	calculateInverseMatrix();

    Vector4 viewVector = mInverseProjectionMatrix.mulVector(Vector4(screenPosition.x, screenPosition.y, -1.0f, 1.0f));
    Vector3 distanceToCameraVector = mViewMatrix.mulVector(Vector4(0,0,depth,1));
    distanceToCameraVector = -distanceToCameraVector;
    // intersect view vector with object Z plane (in view)
    Vector4 view_space_intersect = Vector4(Vector3(viewVector) * distanceToCameraVector.z, 1.0f);
    Vector4 point_world = mInverseViewMatrix.mulVector(view_space_intersect);

	return point_world;
}

Vector2 GPUCamera::worldToScreen(const Vector3& worldPosition)
{	
    calculateProjectionViewMatrix();
	Vector4 v = mProjectionViewMatrix.mulVector(Vector4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0));

    Vector2 result = v;
	result = result / v.w;

	return result;
}

void GPUCamera::setZoom(Core::f32 zoom)
{
	mZoom = zoom;
	recalculateProjectionMatrix();
}

void GPUCamera::zoomIn(Core::f32 zoomDelta)
{
	Core::f32 newZoom = mZoom - zoomDelta;

	if(newZoom < 0)
	{
		newZoom = 0;
	}

	setZoom(newZoom);
}

void GPUCamera::zoomOut(Core::f32 zoomDelta)
{
	Core::f32 newZoom = mZoom + zoomDelta;
	setZoom(newZoom);
}

void GPUCamera::resetZoom()
{
	mZoom = 1;
	setZoom(mZoom);
}

void GPUCamera::calculateProjectionViewMatrix()
{
	mProjectionViewMatrix.init(mProjectionMatrix);
	mProjectionViewMatrix.mul(mViewMatrix);
}

void GPUCamera::calculateInverseMatrix(bool force /*= false*/)
{
	if(mInversePVMatrixNeedsUpdate || force)
	{
		calculateProjectionViewMatrix();

        mInverseProjectionMatrix = mProjectionMatrix;
        mInverseProjectionMatrix.invert();
        mInverseViewMatrix = mViewMatrix;
        mInverseViewMatrix.invert();

		mInversePVMatrix.init(mProjectionViewMatrix);		
		mInversePVMatrix.invert();

		mInversePVMatrixNeedsUpdate = false;
	}
}
