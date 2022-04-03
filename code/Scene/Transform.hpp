#pragma once

#include "Scene/Component.hpp"

#ifdef CPP_INCLUDE
#include "Scene/Transform.hpp"
#include "Scene/GameObject.hpp"
#endif

class Transform;

class TransformState : public ObjectBase
{
    GENERATE_METADATA(TransformState)

private:
	Vector3 mWorldPosition;
	Vector3 mRotation;
	Vector3 mScale;

public:
	TransformState() = default;
	CPP TransformState(const Transform& transform)
	{
		mWorldPosition = transform.getWorldPosition();
		mRotation = transform.getRotation();
		mScale = transform.getScale();
	}

	bool eq(const TransformState& rhs, f32 eps = MathUtils::FLOAT_EPSILON) const
	{
		return mWorldPosition.eq(rhs.mWorldPosition, eps) &&
			mRotation.eq(rhs.mRotation, eps) &&
			mScale.eq(rhs.mScale, eps);
	}

	CRGET(WorldPosition)
	CRGET(Rotation)
	CRGET(Scale)
};

class Transform: public Component
{
    GENERATE_METADATA(Transform)
	
private:
	mutable Vector3 mWorldPosition;

	mutable Matrix4 mModelMatrix;
	mutable Matrix4 mTranslationMatrix;
	mutable Matrix4 mRotationMatrix;
	mutable Matrix4 mScaleMatrix;

	mutable bool mModelMatrixGenerated = false;

	Transform* mParent = nullptr;

	Vector3 mLocalPosition;
	Vector3 mRotation;
	Vector3 mScale;
	bool mAffectedByProjection = true;

public:
	static const Vector3 smRight;
	static const Vector3 smUp;
	static const Vector3 smForward;

	CPP void init() override
	{
		// TRACE();
		mTranslationMatrix.identity();
		mRotationMatrix.identity();
		mScaleMatrix.identity();

		mWorldPosition = Vector3(0.0f, 0.0f, 0.0f);
		mLocalPosition = Vector3(0.0f, 0.0f, 0.0f);
		mRotation = Vector3(0.0f, 0.0f, 0.0f);
		mScale = Vector3(1.0f, 1.0f, 1.0f);

		mModelMatrixGenerated = false;
	}

	CPP const Vector3& getWorldPosition() const
	{
		mWorldPosition = mLocalPosition;

		if (mParent)
		{
			// TODO : take the whole model matrix? or ignore scale?
			//mWorldPosition = mParent->getModelMatrix().mulVector(Vector4(mWorldPosition, 1));

			mWorldPosition.add(mParent->getWorldPosition());

			Matrix4 rotationMatrix;
			rotationMatrix.init(mParent->getRotationMatrix());

			mWorldPosition = Vector3(rotationMatrix.mulVector(Vector4(mWorldPosition, 1.0f)));
		}

		return mWorldPosition;
	}

	CPP void lookAt(const Vector3& targetPosition)
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

	CPP const Matrix4& getTranslationMatrix() const
	{
		mTranslationMatrix.translation(getWorldPosition());
		return mTranslationMatrix;
	}

	CPP const Matrix4& getRotationMatrix() const
	{
		mRotationMatrix.rotation(mRotation);
		return mRotationMatrix;
	}

	CPP const Matrix4& getScaleMatrix() const
	{
		mScaleMatrix.scale(mScale);
		return mScaleMatrix;
	}

	CPP const Matrix4& getModelMatrix(bool force = false) const
	{
		if (!isStatic() || (isStatic() && !mModelMatrixGenerated) || force)
		{
			mModelMatrix.init(getTranslationMatrix());
			Matrix4 rotationMatrix(getRotationMatrix());
			Matrix4 scaleMatrix(getScaleMatrix());

			scaleMatrix.mul(rotationMatrix);
			mModelMatrix.mul(scaleMatrix);

			mModelMatrixGenerated = true;
		}

		return mModelMatrix;
	}

	void translate(const Vector3& vector)
	{
		setLocalPosition(mLocalPosition.add(vector));
	}

	void rotate(const Vector3& vector)
	{
		setRotation(mRotation.add(vector));
	}

	CPP void serialize(JSON& json) const override
	{
		Component::serialize(json);

		DO_SERIALIZE("local_position", mLocalPosition);
		DO_SERIALIZE("scale", mScale);
		DO_SERIALIZE("rotation", mRotation);
	}

	CPP void deserialize(const JSON& json) override
	{
		Component::deserialize(json);

		DO_DESERIALIZE("local_position", mLocalPosition);
		DO_DESERIALIZE("scale", mScale);
		DO_DESERIALIZE("rotation", mRotation);
	}

	GET_SET(Parent)
	CRGET_SET(LocalPosition)
	CRGET_SET(Rotation)
	CRGET_SET(Scale)
	GET_SET(AffectedByProjection)
};