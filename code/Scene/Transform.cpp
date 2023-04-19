#include "Scene/Transform.hpp"
#include "Scene/GameObject.hpp"


TransformState::TransformState(const Transform& transform)
{
	mPosition = transform.getWorldPosition();
	mRotation = transform.mRotation;
	mScale = transform.mScale;
}

TransformState::TransformState(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;
}

void Transform::init() 
{
	mTranslationMatrix.identity();
	mRotationMatrix.identity();
	mScaleMatrix.identity();

	mWorldPosition = Vector3(0.0f, 0.0f, 0.0f);
	mLocalPosition = Vector3(0.0f, 0.0f, 0.0f);
	mRotation = Vector3(0.0f, 0.0f, 0.0f);
	mScale = Vector3(1.0f, 1.0f, 1.0f);

	mModelMatrixGenerated = false;
}

const Vector3& Transform::getWorldPosition() const
{
	mWorldPosition = mLocalPosition;

	if (mParent)
	{
		mWorldPosition.add(mParent->getWorldPosition());

		Matrix4 rotationMatrix;
		rotationMatrix.init(mParent->getRotationMatrix());

		mWorldPosition = Vector3(rotationMatrix.mulVector(Vector4(mWorldPosition, 1.0f)));
	}

	return mWorldPosition;
}

void Transform::lookAt(const Vector3& targetPosition)
{
	/*mIsDirtyRotation = true;

	Vector3 target(targetPosition);

	Vector3 forward(target.sub(mLocalPosition).nor());

	Vector3 yAxis(0, 1, 0);
	Vector3 right(yAxis.cross(forward).nor());
	Vector3 up(Vector3(forward).cross(right));

	Matrix4 lookAtMatrix;
	lookAtMatrix.init(Vector4(right.x, right.y, right.z, 0), Vector4(up.x, up.y, up.z, 0),
			Vector4(forward.x, forward.y, forward.z, 0), Vector4(0, 0, 0, 1));

	Quaternion q;

	q.fromMatrix(lookAtMatrix);

	mRotation = q.toEuler();*/
}

const Matrix4& Transform::getTranslationMatrix() const
{
	mTranslationMatrix.translation(getWorldPosition());
	return mTranslationMatrix;
}

const Matrix4& Transform::getRotationMatrix() const
{
	mRotationMatrix.rotation(mRotation);
	return mRotationMatrix;
}

const Matrix4& Transform::getScaleMatrix() const
{
	mScaleMatrix.scale(mScale);
	return mScaleMatrix;
}

const Matrix4& Transform::getModelMatrix(bool force /*= false*/) const
{
	if (!isStatic() || (isStatic() and !mModelMatrixGenerated) || force)
	{
		mModelMatrix.init(getTranslationMatrix());
		Matrix4 rotationMatrix(getRotationMatrix());
		Matrix4 scaleMatrix(getScaleMatrix());

		rotationMatrix.mul(scaleMatrix);
		mModelMatrix.mul(rotationMatrix);

		mModelMatrixGenerated = true;
	}

	return mModelMatrix;
}

IMPLEMENT_SERIALIZATION(Transform)
{
	Component::serialize(json);

	SERIALIZE("local_position", mLocalPosition);
	SERIALIZE("scale", mScale);
	SERIALIZE("rotation", mRotation);
}

IMPLEMENT_DESERIALIZATION(Transform)
{
	Component::deserialize(json);

	DESERIALIZE("local_position", mLocalPosition);
	DESERIALIZE("scale", mScale);
	DESERIALIZE("rotation", mRotation);
}
