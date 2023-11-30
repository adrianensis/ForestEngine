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
		return mPosition.eq(rhs.mPosition, eps) and
			mRotation.eq(rhs.mRotation, eps) and
			mScale.eq(rhs.mScale, eps);
	}

private:
	Vector3 mPosition = Vector3(0,0,0);
	Vector3 mRotation = Vector3(0,0,0);
	Vector3 mScale = Vector3(1,1,1);

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
    void init();
    void onDestroy() override;
    Vector3 getWorldPosition() const;
    Vector3 getWorldScale() const;
    Vector3 getWorldRotation() const;
    void lookAt(const Vector3& targetPosition);
    const Matrix4& getTranslationMatrix() const;
    const Matrix4& getRotationMatrix() const;
    const Matrix4& getScaleMatrix() const;
    const Matrix4& calculateModelMatrix() const;

	void translate(const Vector3& vector);
	void rotate(const Vector3& vector);

    void setPosition(const Vector3& vec);
	void setRotation(const Vector3& vec);
	void setScale(const Vector3& vec);

    void addChild(Ptr<Transform> child);
    void removeChild(Ptr<Transform> child);

private:
    void notifyModelMatrixDirty();

private:
    std::unordered_map<ObjectId, Ptr<Transform>> mChildren;
    Ptr<Transform> mParent;
	
    mutable bool mModelMatrixDirty = true;

	Vector3 mLocalPosition = Vector3(0,0,0);
	Vector3 mRotation = Vector3(0,0,0);
	Vector3 mScale = Vector3(1,1,1);

	mutable Matrix4 mModelMatrix;
	mutable Matrix4 mModelMatrixNoScale;
	mutable Matrix4 mTranslationMatrix;
	mutable Matrix4 mRotationMatrix;
	mutable Matrix4 mScaleMatrix;

public:
	bool mAffectedByProjection = true;
    bool mIgnoreParentScale = true;

public:
    CRGET(LocalPosition)
    CRGET(Rotation)
    CRGET(Scale)
    GET(ModelMatrixDirty)
    GET(ModelMatrixNoScale)
    GET_SET(IgnoreParentScale)
};
