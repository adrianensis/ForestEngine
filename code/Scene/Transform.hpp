#pragma once

#include "Scene/Component.hpp"

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
	TransformState(const Transform& transform);

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

	bool mModelMatrixGenerated = false;

	Transform* mParent = nullptr;

	Vector3 mLocalPosition;
	Vector3 mRotation;
	Vector3 mScale;
	bool mAffectedByProjection = true;

public:
	static const Vector3 smRight;
	static const Vector3 smUp;
	static const Vector3 smForward;

	void init() override;

	void lookAt(const Vector3& targetPosition);

	const Vector3& getWorldPosition() const;

	const Matrix4& getTranslationMatrix() const;
	const Matrix4& getRotationMatrix() const;
	const Matrix4& getScaleMatrix() const;

	const Matrix4& getModelMatrix(bool force = false);

	void translate(const Vector3& vector)
	{
		setLocalPosition(mLocalPosition.add(vector));
	}

	void rotate(const Vector3& vector)
	{
		setRotation(mRotation.add(vector));
	}

	void serialize(JSON& json) const override;
	void deserialize(const JSON& json) override;

	GET_SET(Parent)
	CRGET_SET(LocalPosition)
	CRGET_SET(Rotation)
	CRGET_SET(Scale)
	GET_SET(AffectedByProjection)
};