#pragma once

#include "Scene/Component.hpp"

class Transform;

class TransformState : public ObjectBase
{
    GENERATE_METADATA(TransformState)

public:
	Vector3 mWorldPosition;
	Vector3 mRotation;
	Vector3 mScale;

	TransformState() = default;
	TransformState(const Transform& transform);

	bool eq(const TransformState& rhs, f32 eps = MathUtils::FLOAT_EPSILON) const
	{
		return mWorldPosition.eq(rhs.mWorldPosition, eps) &&
			mRotation.eq(rhs.mRotation, eps) &&
			mScale.eq(rhs.mScale, eps);
	}

	GET_RC(WorldPosition)
	GET_RC(Rotation)
	GET_RC(Scale)
};

class Transform: public Component
{
    GENERATE_METADATA(Transform)
	PRI mutable Vector3 mWorldPosition;

	PRI mutable Matrix4 mModelMatrix;
	PRI mutable Matrix4 mTranslationMatrix;
	PRI mutable Matrix4 mRotationMatrix;
	PRI mutable Matrix4 mScaleMatrix;

	PRI bool mModelMatrixGenerated = false;

	//Transform* mParent;
	PRI Transform* mParent = nullptr;

	PRI Vector3 mLocalPosition;
	PRI Vector3 mRotation;
	PRI Vector3 mScale;
	PRI bool mAffectedByProjection = false;

public:
	static const Vector3 smRight;
	static const Vector3 smUp;
	static const Vector3 smForward;

	Transform();
	~Transform() override;

	void init() override;

	/*!
	\brief Translate the object by an increment vector.
	\param Vector3 Delta position.
	*/
	void translate(const Vector3& vector);

	/*!
	\brief Rotate the object by an increment vector.
	\param Vector3 Delta rotation.
	*/
	void rotate(const Vector3& vector);

	/*!
	\brief Orient the object to a 3D point.
	\param Vector3 Target position.
	*/
	void lookAt(const Vector3& targetPosition);

	// void setWorldPosition(const Vector3& vector);
	void setLocalPosition(const Vector3& vector);
	void setRotation(const Vector3& vector);
	void setScale(const Vector3& vector);

	const Vector3& getWorldPosition() const;

	const Matrix4& getTranslationMatrix() const;
	const Matrix4& getRotationMatrix() const;
	const Matrix4& getScaleMatrix() const;

	const Matrix4& getModelMatrix(bool force = false);

	TransformState getTransformState() const;

	void serialize(JSON& json) const override;
	void deserialize(const JSON& json) override;

	GET_SET(Parent)
	GET_RC(LocalPosition)
	GET_RC(Rotation)
	GET_RC(Scale)
	GET_SET(AffectedByProjection)
};