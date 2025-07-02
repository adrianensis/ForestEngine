#include "GPU/Camera/GPUFrustum.hpp"
#include "Scene/Module.hpp"
#include "GPU/Camera/GPUCamera.hpp"


void GPUFrustum::init(GPUCamera *camera)
{
	mCamera = camera;

	for (Core::u32 i = 0; i < mPlanes.size(); ++i)
	{
		mPlanes[i] = Maths::Vector4(0, 0, 0, 0); // x,y,z,w <=> A,B,C,D
	}

	mVPmatrix.identity();
};

bool GPUFrustum::testSphere(const Maths::Vector3& center, Core::f32 radius) const
{
	bool result = true;

	FOR_RANGE_COND(i, 0, mPlanes.size(), result)
	{
		Core::f32 A = mPlanes.at(i).x;
		Core::f32 B = mPlanes.at(i).y;
		Core::f32 C = mPlanes.at(i).z;
		Core::f32 D = mPlanes.at(i).w;

		if ((A * center.x) + (B * center.y) + (C * center.z) + D <= -radius)
			result = false;
	}

	return result;
};

bool GPUFrustum::testPoint(const Maths::Vector3& point) const
{
	bool result = true;

	FOR_RANGE_COND(i, 0, mPlanes.size(), result)
	{
		if (Maths::Vector3(mPlanes.at(i)).dst(point) < 0)
			result = false;
	}

	return result;
}

bool GPUFrustum::testRectangle(const Maths::Vector3& leftTop, Core::f32 width, Core::f32 height) const
{
	return testPoint(leftTop) || testPoint(Maths::Vector3(leftTop.x, leftTop.y - height, 0)) || testPoint(Maths::Vector3(leftTop.x + width, leftTop.y - height, 0)) || testPoint(Maths::Vector3(leftTop.x + width, leftTop.y, 0));
}

void GPUFrustum::build()
{
	Core::u32 LEFT = 0;
	Core::u32 RIGHT = 1;
	Core::u32 BOTTOM = 2;
	Core::u32 TOP = 3;
	Core::u32 NEAR = 4;
	Core::u32 FAR = 5;

	mVPmatrix.init(mCamera->mProjectionMatrix);
	mVPmatrix.mul(mCamera->mViewMatrix);

	mPlanes[LEFT] =
		Maths::Vector4(mVPmatrix.get(0, 0) + mVPmatrix.get(3, 0), mVPmatrix.get(0, 1) + mVPmatrix.get(3, 1),
				mVPmatrix.get(0, 2) + mVPmatrix.get(3, 2), mVPmatrix.get(0, 3) + mVPmatrix.get(3, 3));

	mPlanes[RIGHT] =
		Maths::Vector4(-mVPmatrix.get(0, 0) + mVPmatrix.get(3, 0), -mVPmatrix.get(0, 1) + mVPmatrix.get(3, 1),
				-mVPmatrix.get(0, 2) + mVPmatrix.get(3, 2), -mVPmatrix.get(0, 3) + mVPmatrix.get(3, 3));

	mPlanes[BOTTOM] =
		Maths::Vector4(mVPmatrix.get(1, 0) + mVPmatrix.get(3, 0), mVPmatrix.get(1, 1) + mVPmatrix.get(3, 1),
				mVPmatrix.get(1, 2) + mVPmatrix.get(3, 2), mVPmatrix.get(1, 3) + mVPmatrix.get(3, 3));

	mPlanes[TOP] =
		Maths::Vector4(-mVPmatrix.get(1, 0) + mVPmatrix.get(3, 0), -mVPmatrix.get(1, 1) + mVPmatrix.get(3, 1),
				-mVPmatrix.get(1, 2) + mVPmatrix.get(3, 2), -mVPmatrix.get(1, 3) + mVPmatrix.get(3, 3));

	mPlanes[NEAR] =
		Maths::Vector4(mVPmatrix.get(2, 0) + mVPmatrix.get(3, 0), mVPmatrix.get(2, 1) + mVPmatrix.get(3, 1),
				mVPmatrix.get(2, 2) + mVPmatrix.get(3, 2), mVPmatrix.get(2, 3) + mVPmatrix.get(3, 3));

	mPlanes[FAR] =
		Maths::Vector4(-mVPmatrix.get(2, 0) + mVPmatrix.get(3, 0), -mVPmatrix.get(2, 1) + mVPmatrix.get(3, 1),
				-mVPmatrix.get(2, 2) + mVPmatrix.get(3, 2), -mVPmatrix.get(2, 3) + mVPmatrix.get(3, 3));

	for (Core::u32 i = 0; i < mPlanes.size(); ++i)
	{
		Maths::Vector4 v4(mPlanes.at(i));
		Maths::Vector3 v3(v4.x, v4.y, v4.z);

		if (v3.len() > 0)
		{
			v4.x = v4.x / v3.len();
			v4.y = v4.y / v3.len();
			v4.z = v4.z / v3.len();
			v4.w = v4.w / v3.len();


			mPlanes[i] = v4;
		}
	}
}
