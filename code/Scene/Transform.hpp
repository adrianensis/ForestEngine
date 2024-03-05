#pragma once

#include "Scene/Component.hpp"

class Transform: public Component
{
    
	DECLARE_SERIALIZATION()

public:
    void init();
    void onDestroy() override;

	void addLocalTranslation(const Vector3& vector);
	void addLocalRotation(const Vector3& vector);
	void addLocalScale(const Vector3& vector);
    void lookAt(const Vector3& targetPosition);

    const Matrix4& calculateModelMatrix() const;

    void addChild(Ptr<Transform> child);
    void removeChild(Ptr<Transform> child);

    Vector3 getWorldPosition() const;
    Vector3 getWorldScale() const;
    Vector3 getWorldRotation() const;
    const Matrix4& getLocalTranslationMatrix() const;
    const Matrix4& getLocalRotationMatrix() const;
    const Matrix4& getLocalScaleMatrix() const;
    void setLocalPosition(const Vector3& vec);
	void setLocalRotation(const Vector3& vec);
	void setLocalScale(const Vector3& vec);

private:
    void notifyModelMatrixDirty();

private:
    std::unordered_map<ObjectId, Ptr<Transform>> mChildren;
    Ptr<Transform> mParent;
	
    mutable bool mModelMatrixDirty = true;
    mutable bool mLocalTranslationMatrixDirty = true;
    mutable bool mLocalRotationMatrixDirty = true;
    mutable bool mLocalScaleMatrixDirty = true;

	Vector3 mLocalPosition = Vector3::smZero;
	Vector3 mLocalRotation = Vector3::smZero;
	Vector3 mLocalScale = Vector3::smOne;

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
    CRGET(LocalRotation)
    CRGET(LocalScale)
    GET(ModelMatrixNoScale)
    GET_SET(IgnoreParentScale)
};
REGISTER_CLASS(Transform);
