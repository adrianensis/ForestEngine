#pragma once

#include "Engine/Core.hpp"
#include "Engine/EntityComponent/Component.hpp"

class Transform: public EC::Component
{
    
public:
    void init();
    void onDestroy() override;

	void addLocalTranslation(const Maths::Vector3& vector);
	void addLocalRotation(const Maths::Vector3& vector);
	void addLocalScale(const Maths::Vector3& vector);
    void lookAt(const Maths::Vector3& targetPosition);

    const Maths::Matrix4& calculateModelMatrix() const;

    void addChild(EC::ComponentPtr<Transform> child);
    void removeChild(EC::ComponentPtr<Transform> child);

    Maths::Vector3 getWorldPosition() const;
    Maths::Vector3 getWorldScale() const;
    Maths::Vector3 getWorldRotation() const;
    const Maths::Matrix4& getLocalTranslationMatrix() const;
    const Maths::Matrix4& getLocalRotationMatrix() const;
    const Maths::Matrix4& getLocalScaleMatrix() const;
    void setLocalPosition(const Maths::Vector3& vec);
	void setLocalRotation(const Maths::Vector3& vec);
	void setLocalScale(const Maths::Vector3& vec);
	void setBaseModelMatrix(const Maths::Matrix4& matrix);

    const Maths::Matrix4& getViewMatrix() const;

private:
    void notifyModelMatrixDirty();
    
private:
    std::unordered_map<Core::ObjectId, EC::ComponentPtr<Transform>> mChildren;
    EC::ComponentPtr<Transform> mParent;
	
    mutable bool mModelMatrixDirty = true;
    mutable bool mLocalTranslationMatrixDirty = true;
    mutable bool mLocalRotationMatrixDirty = true;
    mutable bool mLocalScaleMatrixDirty = true;
    mutable bool mWorldTranslationMatrixDirty = true;
    mutable bool mWorldRotationMatrixDirty = true;
    mutable bool mWorldScaleMatrixDirty = true;
    mutable bool mViewMatrixDirty = true;

	Maths::Vector3 mLocalPosition = Maths::Vector3::smZero;
	Maths::Vector3 mLocalRotation = Maths::Vector3::smZero;
	Maths::Vector3 mLocalScale = Maths::Vector3::smOne;
	mutable Maths::Vector3 mWorldPosition = Maths::Vector3::smZero;
	mutable Maths::Vector3 mWorldRotation = Maths::Vector3::smZero;
	mutable Maths::Vector3 mWorldScale = Maths::Vector3::smOne;

	Maths::Matrix4 mBaseModelMatrix = Maths::Matrix4::smIdentity;
	mutable Maths::Matrix4 mModelMatrix;
	mutable Maths::Matrix4 mModelMatrixNoScale;
	mutable Maths::Matrix4 mTranslationMatrix;
	mutable Maths::Matrix4 mRotationMatrix;
	mutable Maths::Matrix4 mScaleMatrix;
	mutable Maths::Matrix4 mViewMatrix;

public:
    bool mIgnoreParentScale = true;

public:
    CRGET(LocalPosition)
    CRGET(LocalRotation)
    CRGET(LocalScale)
    GET(ModelMatrixNoScale)
    GET(ModelMatrixDirty)
    GET_SET(IgnoreParentScale)
    CRGET(BaseModelMatrix)
};
REGISTER_CLASS(Transform);
