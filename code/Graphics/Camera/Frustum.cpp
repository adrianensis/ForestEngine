#include "Graphics/Camera/Frustum.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Camera/Camera.hpp"


void Frustum::init(Camera *camera)
{
	TRACE()

	mCamera = camera;

	for (u32 i = 0; i < mPlanes.size(); ++i)
	{
		mPlanes[i] = Vector4(0, 0, 0, 0); // x,y,z,w <=> A,B,C,D
	}

	mVPmatrix.identity();
};

bool Frustum::testSphere(const Vector3& center, f32 radius) const
{
	bool result = true;

	FOR_RANGE_COND(i, 0, mPlanes.size(), result)
	{
		f32 A = mPlanes.at(i).x;
		f32 B = mPlanes.at(i).y;
		f32 C = mPlanes.at(i).z;
		f32 D = mPlanes.at(i).w;

		if ((A * center.x) + (B * center.y) + (C * center.z) + D <= -radius)
			result = false;
	}

	return result;
};

bool Frustum::testPoint(const Vector3& point) const
{
	bool result = true;

	FOR_RANGE_COND(i, 0, mPlanes.size(), result)
	{
		if (Vector3(mPlanes.at(i)).dst(point) < 0)
			result = false;
	}

	return result;
}

bool Frustum::testRectangle(const Vector3& leftTop, f32 width, f32 height) const
{
	return testPoint(leftTop) || testPoint(Vector3(leftTop.x, leftTop.y - height, 0)) || testPoint(Vector3(leftTop.x + width, leftTop.y - height, 0)) || testPoint(Vector3(leftTop.x + width, leftTop.y, 0));
}

void Frustum::build()
{
	u32 LEFT = 0;
	u32 RIGHT = 1;
	u32 BOTTOM = 2;
	u32 TOP = 3;
	u32 NEAR = 4;
	u32 FAR = 5;

	mVPmatrix.init(mCamera->mProjectionMatrix);
	mVPmatrix.mul(mCamera->mViewMatrix);

	mPlanes[LEFT] =
		Vector4(mVPmatrix.get(0, 0) + mVPmatrix.get(3, 0), mVPmatrix.get(0, 1) + mVPmatrix.get(3, 1),
				mVPmatrix.get(0, 2) + mVPmatrix.get(3, 2), mVPmatrix.get(0, 3) + mVPmatrix.get(3, 3));

	mPlanes[RIGHT] =
		Vector4(-mVPmatrix.get(0, 0) + mVPmatrix.get(3, 0), -mVPmatrix.get(0, 1) + mVPmatrix.get(3, 1),
				-mVPmatrix.get(0, 2) + mVPmatrix.get(3, 2), -mVPmatrix.get(0, 3) + mVPmatrix.get(3, 3));

	mPlanes[BOTTOM] =
		Vector4(mVPmatrix.get(1, 0) + mVPmatrix.get(3, 0), mVPmatrix.get(1, 1) + mVPmatrix.get(3, 1),
				mVPmatrix.get(1, 2) + mVPmatrix.get(3, 2), mVPmatrix.get(1, 3) + mVPmatrix.get(3, 3));

	mPlanes[TOP] =
		Vector4(-mVPmatrix.get(1, 0) + mVPmatrix.get(3, 0), -mVPmatrix.get(1, 1) + mVPmatrix.get(3, 1),
				-mVPmatrix.get(1, 2) + mVPmatrix.get(3, 2), -mVPmatrix.get(1, 3) + mVPmatrix.get(3, 3));

	mPlanes[NEAR] =
		Vector4(mVPmatrix.get(2, 0) + mVPmatrix.get(3, 0), mVPmatrix.get(2, 1) + mVPmatrix.get(3, 1),
				mVPmatrix.get(2, 2) + mVPmatrix.get(3, 2), mVPmatrix.get(2, 3) + mVPmatrix.get(3, 3));

	mPlanes[FAR] =
		Vector4(-mVPmatrix.get(2, 0) + mVPmatrix.get(3, 0), -mVPmatrix.get(2, 1) + mVPmatrix.get(3, 1),
				-mVPmatrix.get(2, 2) + mVPmatrix.get(3, 2), -mVPmatrix.get(2, 3) + mVPmatrix.get(3, 3));

	for (u32 i = 0; i < mPlanes.size(); ++i)
	{
		Vector4 v4(mPlanes.at(i));
		Vector3 v3(v4.x, v4.y, v4.z);

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
