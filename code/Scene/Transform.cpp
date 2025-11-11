#include "Scene/Transform.hpp"
#include "Engine/EntityComponent/EntityComponentManager.hpp"

void Transform::init() 
{
    PROFILER_CPU()
	mTranslationMatrix.identity();
	mRotationMatrix.identity();
	mScaleMatrix.identity();

	mLocalPosition = Maths::Vector3(0.0f, 0.0f, 0.0f);
	mLocalRotation = Maths::Vector3(0.0f, 0.0f, 0.0f);
	mLocalScale = Maths::Vector3(1.0f, 1.0f, 1.0f);

	notifyModelMatrixDirty();
}

void Transform::onDestroy()
{
    if(mParent)
    {
        mParent->removeChild(ECManager.getComponentPtr(this));
    }
}

Maths::Vector3 Transform::getWorldPosition() const
{
    if(mWorldTranslationMatrixDirty)
    {
        mWorldPosition = mLocalPosition;
        if (mParent)
        {
            const Maths::Matrix4& parentModelMatrix = mParent->calculateModelMatrix();
            mWorldPosition = parentModelMatrix.mulVector(Maths::Vector4(mWorldPosition, 1.0f));
        }

        mWorldTranslationMatrixDirty = false;
    }

	return mWorldPosition;
}

Maths::Vector3 Transform::getWorldScale() const
{
    if(mWorldScaleMatrixDirty)
    {
        mWorldScale = mLocalScale;
        if (mParent)
        {
            const Maths::Matrix4& parentModelMatrix = mParent->calculateModelMatrix();
            mWorldScale = parentModelMatrix.mulVector(Maths::Vector4(mWorldScale, 1.0f));
        }

        mWorldScaleMatrixDirty = false;
    }

	return mWorldScale;
}

Maths::Vector3 Transform::getWorldRotation() const
{
    if(mWorldRotationMatrixDirty)
    {
        mWorldRotation = mLocalRotation;
        if (mParent)
        {
            const Maths::Matrix4& parentModelMatrix = mParent->calculateModelMatrix();
            mWorldRotation = parentModelMatrix.mulVector(Maths::Vector4(mWorldRotation, 1.0f));
        }

        mWorldRotationMatrixDirty = false;
    }

	return mWorldRotation;
}

void Transform::lookAt(const Maths::Vector3& targetPosition)
{
    Maths::Vector3 mWorldPosition = getWorldPosition();
    Maths::Matrix4 lookAt;
    lookAt.lookAt(mWorldPosition, targetPosition);

	Maths::Quaternion q;
	q.fromMatrix(lookAt);
	setLocalRotation(q.toEuler());
}

const Maths::Matrix4& Transform::getLocalTranslationMatrix() const
{
    if(mLocalTranslationMatrixDirty)
    {
	    mTranslationMatrix.translation(mLocalPosition);
        mLocalTranslationMatrixDirty = false;
    }
	return mTranslationMatrix;
}

const Maths::Matrix4& Transform::getLocalRotationMatrix() const
{
    if(mLocalRotationMatrixDirty)
    {
	    mRotationMatrix.rotation(mLocalRotation);
        mLocalRotationMatrixDirty = false;
    }
	return mRotationMatrix;
}

const Maths::Matrix4& Transform::getLocalScaleMatrix() const
{
    if(mLocalScaleMatrixDirty)
    {
	    mScaleMatrix.scale(mLocalScale);
        mLocalScaleMatrixDirty = false;
    }
	return mScaleMatrix;
}

const Maths::Matrix4& Transform::calculateModelMatrix() const
{
    PROFILER_CPU()

    if(mModelMatrixDirty)
    {
        Maths::Matrix4 translationMatrix = getLocalTranslationMatrix();
        Maths::Matrix4 rotationMatrix = getLocalRotationMatrix();
        Maths::Matrix4 scaleMatrix = getLocalScaleMatrix();
        mModelMatrix = Maths::Matrix4::transform(translationMatrix, rotationMatrix, scaleMatrix);
        mModelMatrix.mul(mBaseModelMatrix);
        mModelMatrixNoScale = Maths::Matrix4::transform(translationMatrix, rotationMatrix, Maths::Matrix4::smIdentity);
        mModelMatrixNoScale.mul(mBaseModelMatrix);

        if (mParent)
        {
            Maths::Matrix4 parentModelMatrix = mParent->calculateModelMatrix();
            if(mIgnoreParentScale)
            {
                parentModelMatrix = mParent->getModelMatrixNoScale();
            }
            parentModelMatrix.mul(mModelMatrix);
            mModelMatrix = parentModelMatrix;
            
            Maths::Matrix4 parentModelMatrixNoScale = mParent->getModelMatrixNoScale();
            parentModelMatrixNoScale.mul(mModelMatrixNoScale);
            mModelMatrixNoScale = parentModelMatrixNoScale;
        }

        mModelMatrixDirty = false;
    }

	return mModelMatrix;
}

void Transform::notifyModelMatrixDirty()
{
    PROFILER_CPU()
    mViewMatrixDirty = true;
    mModelMatrixDirty = true;
    mWorldTranslationMatrixDirty = true;
    mWorldRotationMatrixDirty = true;
    mWorldScaleMatrixDirty = true;
    FOR_MAP(it, mChildren)
    {
        it->second->notifyModelMatrixDirty();
    }
}

void Transform::addLocalTranslation(const Maths::Vector3& vector)
{
    setLocalPosition(mLocalPosition.add(vector));
}

void Transform::addLocalRotation(const Maths::Vector3& vector)
{
    setLocalRotation(mLocalRotation.add(vector));
}

void Transform::addLocalScale(const Maths::Vector3& vector)
{
    setLocalScale(mLocalScale.add(vector));
}

void Transform::setLocalPosition(const Maths::Vector3& vec)
{
    mLocalPosition = vec;
    mLocalTranslationMatrixDirty = true;
    notifyModelMatrixDirty();
}

void Transform::setLocalRotation(const Maths::Vector3& vec)
{
    mLocalRotation = vec;
    mLocalRotation.x = std::fmod(mLocalRotation.x, 360.0f);
    mLocalRotation.x = mLocalRotation.x < 0.0f ? 360.0f + mLocalRotation.x : mLocalRotation.x;
    mLocalRotation.y = std::fmod(mLocalRotation.y, 360.0f);
    mLocalRotation.y = mLocalRotation.y < 0.0f ? 360.0f + mLocalRotation.y : mLocalRotation.y;
    mLocalRotation.z = std::fmod(mLocalRotation.z, 360.0f);
    mLocalRotation.z = mLocalRotation.z < 0.0f ? 360.0f + mLocalRotation.z : mLocalRotation.z;
    mLocalRotationMatrixDirty = true;
    notifyModelMatrixDirty();
}

void Transform::setLocalScale(const Maths::Vector3& vec)
{
    mLocalScale = vec;
    mLocalScaleMatrixDirty = true;
    notifyModelMatrixDirty();
}

void Transform::setBaseModelMatrix(const Maths::Matrix4& matrix)
{
    mBaseModelMatrix = matrix;
    notifyModelMatrixDirty();
}

const Maths::Matrix4& Transform::getViewMatrix() const
{
    if(mViewMatrixDirty)
    {
        Maths::Vector3 worldPosition = ECManager.getFirstComponent<Transform>(getOwnerEntity())->getWorldPosition();
        const Maths::Matrix4& rotationMatrix = ECManager.getFirstComponent<Transform>(getOwnerEntity())->getLocalRotationMatrix();
        mViewMatrix.view(worldPosition, rotationMatrix);
        mViewMatrixDirty = false;
    }

    return mViewMatrix;
}

void Transform::addChild(EC::TComponentPtr<Transform> child)
{
    child->mParent = ECManager.getComponentPtr(this);
    mChildren.insert_or_assign(child->getComponentId(), child);
}

void Transform::removeChild(EC::TComponentPtr<Transform> child)
{
    child->mParent.reset();
    mChildren.erase(child->getComponentId());
}