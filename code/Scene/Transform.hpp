#pragma once

#include "Scene/Component.hpp"


class Transform;

class TransformState : public ObjectBase
{
    GENERATE_METADATA(TransformState)

public:
	TransformState() = default;
    TransformState(const Transform& transform);
    TransformState(const Vector3& position, const Vector3& rotation, const Vector3& scale);

	bool eq(const TransformState& rhs, f32 eps = MathUtils::FLOAT_EPSILON) const
	{
		return mPosition.eq(rhs.mPosition, eps) &&
			mRotation.eq(rhs.mRotation, eps) &&
			mScale.eq(rhs.mScale, eps);
	}

private:
	Vector3 mPosition;
	Vector3 mRotation;
	Vector3 mScale;

public:
	CRGET(Position)
	CRGET(Rotation)
	CRGET(Scale)
};

class Transform: public Component
{
    GENERATE_METADATA(Transform)
	DECLARE_SERIALIZATION()

public:
	static const Vector3 smRight;
	static const Vector3 smUp;
	static const Vector3 smForward;
    void init() override;
    const Vector3& getWorldPosition() const;
    void lookAt(const Vector3& targetPosition);
    const Matrix4& getTranslationMatrix() const;
    const Matrix4& getRotationMatrix() const;
    const Matrix4& getScaleMatrix() const;
    const Matrix4& getModelMatrix(bool force = false) const;

	void translate(const Vector3& vector)
	{
		mLocalPosition = (mLocalPosition.add(vector));
	}

	void rotate(const Vector3& vector)
	{
		mRotation = (mRotation.add(vector));
	}

private:

	mutable bool mModelMatrixGenerated = false;

public:
	Transform* mParent = nullptr;

	Vector3 mLocalPosition;
	Vector3 mRotation;
	Vector3 mScale;
	bool mAffectedByProjection = true;

	mutable Vector3 mWorldPosition;

	mutable Matrix4 mModelMatrix;
	mutable Matrix4 mTranslationMatrix;
	mutable Matrix4 mRotationMatrix;
	mutable Matrix4 mScaleMatrix;
};
