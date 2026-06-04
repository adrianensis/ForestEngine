
#include "GPU/Camera/GPUCamera.hpp"

void GPUCamera::init()
{
	mIsOrtho = true;
	mZoom = 1;
	
	mViewMatrix.identity();
	mInversePVMatrix.identity();
	mFrustum.init(this);
}

void GPUCamera::update(const Maths::Matrix4& viewMatrix)
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

void GPUCamera::setOrtho(GPU::f32 left, GPU::f32 right, GPU::f32 bottom, GPU::f32 top, GPU::f32 near, GPU::f32 far)
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

void GPUCamera::setPerspective(GPU::f32 near, GPU::f32 far, GPU::f32 aspect, GPU::f32 fov)
{
	mIsOrtho = false;

	mNear = near;
	mFar = far;
	mAspect = aspect;
	mFov = fov;
	
	mProjectionMatrix.perspective(mNear, mFar, mAspect, mFov * mZoom);
}

void GPUCamera::onResize(const Maths::Vector2& windowSize, GPU::f32 aspectRatio)
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

Maths::Vector3 GPUCamera::screenToWorld(const Maths::Vector2& screenPosition, GPU::f32 depth)
{	
	calculateInverseMatrix();

    Maths::Vector4 viewVector = mInverseProjectionMatrix.mulVector(Maths::Vector4(screenPosition.x, screenPosition.y, -1.0f, 1.0f));
    Maths::Vector3 distanceToCameraVector = mViewMatrix.mulVector(Maths::Vector4(0,0,depth,1));
    distanceToCameraVector = -distanceToCameraVector;
    // intersect view vector with object Z plane (in view)
    Maths::Vector4 view_space_intersect = Maths::Vector4(Maths::Vector3(viewVector) * distanceToCameraVector.z, 1.0f);
    Maths::Vector4 point_world = mInverseViewMatrix.mulVector(view_space_intersect);

	return point_world;
}

Maths::Vector2 GPUCamera::worldToScreen(const Maths::Vector3& worldPosition)
{	
    calculateProjectionViewMatrix();
	Maths::Vector4 v = mProjectionViewMatrix.mulVector(Maths::Vector4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0));

    Maths::Vector2 result = v;
	result = result / v.w;

	return result;
}

void GPUCamera::setZoom(GPU::f32 zoom)
{
	mZoom = zoom;
	recalculateProjectionMatrix();
}

void GPUCamera::zoomIn(GPU::f32 zoomDelta)
{
	GPU::f32 newZoom = mZoom - zoomDelta;

	if(newZoom < 0)
	{
		newZoom = 0;
	}

	setZoom(newZoom);
}

void GPUCamera::zoomOut(GPU::f32 zoomDelta)
{
	GPU::f32 newZoom = mZoom + zoomDelta;
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
