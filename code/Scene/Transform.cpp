#include "Scene/Transform.hpp"
#include "Scene/GameObject.hpp"

void Transform::init() 
{
	mTranslationMatrix.identity();
	mRotationMatrix.identity();
	mScaleMatrix.identity();

	mLocalPosition = Vector3(0.0f, 0.0f, 0.0f);
	mLocalRotation = Vector3(0.0f, 0.0f, 0.0f);
	mLocalScale = Vector3(1.0f, 1.0f, 1.0f);

	notifyModelMatrixDirty();
}

void Transform::onDestroy()
{
    if(mParent)
    {
        mParent->removeChild(getPtrToThis<Transform>());
    }
}

Vector3 Transform::getWorldPosition() const
{
    Vector3 worldPosition = mLocalPosition;
    if(mWorldTranslationMatrixDirty)
    {
        if (mParent)
        {
            const Matrix4& parentModelMatrix = mParent->calculateModelMatrix();
            worldPosition = parentModelMatrix.mulVector(Vector4(worldPosition, 1.0f));
        }

        mWorldTranslationMatrixDirty = false;
    }

	return worldPosition;
}

Vector3 Transform::getWorldScale() const
{
    Vector3 worldScale = mLocalScale;
    if(mWorldRotationMatrixDirty)
    {
        if (mParent)
        {
            const Matrix4& parentModelMatrix = mParent->calculateModelMatrix();
            worldScale = parentModelMatrix.mulVector(Vector4(worldScale, 1.0f));
        }

        mWorldRotationMatrixDirty = false;
    }

	return worldScale;
}

Vector3 Transform::getWorldRotation() const
{
    Vector3 worldRotation = mLocalRotation;
    if(mWorldScaleMatrixDirty)
    {
        if (mParent)
        {
            const Matrix4& parentModelMatrix = mParent->calculateModelMatrix();
            worldRotation = parentModelMatrix.mulVector(Vector4(worldRotation, 1.0f));
        }

        mWorldScaleMatrixDirty = false;
    }

	return worldRotation;
}

void Transform::lookAt(const Vector3& targetPosition)
{
    Vector3 mWorldPosition = getWorldPosition();
    Matrix4 lookAt;
    lookAt.lookAt(mWorldPosition, targetPosition);

	Quaternion q;
	q.fromMatrix(lookAt);
	setLocalRotation(q.toEuler());
}

const Matrix4& Transform::getLocalTranslationMatrix() const
{
    if(mLocalTranslationMatrixDirty)
    {
	    mTranslationMatrix.translation(mLocalPosition);
        mLocalTranslationMatrixDirty = false;
    }
	return mTranslationMatrix;
}

const Matrix4& Transform::getLocalRotationMatrix() const
{
    if(mLocalRotationMatrixDirty)
    {
	    mRotationMatrix.rotation(mLocalRotation);
        mLocalRotationMatrixDirty = false;
    }
	return mRotationMatrix;
}

const Matrix4& Transform::getLocalScaleMatrix() const
{
    if(mLocalScaleMatrixDirty)
    {
	    mScaleMatrix.scale(mLocalScale);
        mLocalScaleMatrixDirty = false;
    }
	return mScaleMatrix;
}

const Matrix4& Transform::calculateModelMatrix() const
{
    PROFILER_CPU()

    if(mModelMatrixDirty)
    {
        Matrix4 translationMatrix = getLocalTranslationMatrix();
        Matrix4 rotationMatrix = getLocalRotationMatrix();
        Matrix4 scaleMatrix = getLocalScaleMatrix();
        mModelMatrix = Matrix4::transform(translationMatrix, rotationMatrix, scaleMatrix);
        mModelMatrix.mul(mBaseModelMatrix);
        mModelMatrixNoScale = Matrix4::transform(translationMatrix, rotationMatrix, Matrix4::smIdentity);
        mModelMatrixNoScale.mul(mBaseModelMatrix);

        if (mParent)
        {
            Matrix4 parentModelMatrix = mParent->calculateModelMatrix();
            if(mIgnoreParentScale)
            {
                parentModelMatrix = mParent->getModelMatrixNoScale();
            }
            parentModelMatrix.mul(mModelMatrix);
            mModelMatrix = parentModelMatrix;
            
            Matrix4 parentModelMatrixNoScale = mParent->getModelMatrixNoScale();
            parentModelMatrixNoScale.mul(mModelMatrixNoScale);
            mModelMatrixNoScale = parentModelMatrixNoScale;
        }

        mModelMatrixDirty = false;
    }

	return mModelMatrix;
}

void Transform::notifyModelMatrixDirty()
{
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

void Transform::addLocalTranslation(const Vector3& vector)
{
    setLocalPosition(mLocalPosition.add(vector));
}

void Transform::addLocalRotation(const Vector3& vector)
{
    setLocalRotation(mLocalRotation.add(vector));
}

void Transform::addLocalScale(const Vector3& vector)
{
    setLocalScale(mLocalScale.add(vector));
}

void Transform::setLocalPosition(const Vector3& vec)
{
    mLocalPosition = vec;
    mLocalTranslationMatrixDirty = true;
    notifyModelMatrixDirty();
}

void Transform::setLocalRotation(const Vector3& vec)
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

void Transform::setLocalScale(const Vector3& vec)
{
    mLocalScale = vec;
    mLocalScaleMatrixDirty = true;
    notifyModelMatrixDirty();
}

void Transform::setBaseModelMatrix(const Matrix4& matrix)
{
    mBaseModelMatrix = matrix;
    notifyModelMatrixDirty();
}

const Matrix4& Transform::getViewMatrix() const
{
    if(mViewMatrixDirty)
    {
        Vector3 worldPosition = mGameObject->mTransform->getWorldPosition();
        const Matrix4& rotationMatrix = mGameObject->mTransform->getLocalRotationMatrix();
        mViewMatrix.view(worldPosition, rotationMatrix);
        mViewMatrixDirty = false;
    }

    return mViewMatrix;
}

void Transform::addChild(Ptr<Transform> child)
{
    child->mParent = getPtrToThis<Transform>();
    mChildren.insert_or_assign(child->getObjectId(), child);
}

void Transform::removeChild(Ptr<Transform> child)
{
    child->mParent.invalidate();
    mChildren.erase(child->getObjectId());
}

IMPLEMENT_SERIALIZATION(Transform)
{
	Component::serialize(json);

	SERIALIZE("local_position", mLocalPosition);
	SERIALIZE("scale", mLocalScale);
	SERIALIZE("rotation", mLocalRotation);
}

IMPLEMENT_DESERIALIZATION(Transform)
{
	Component::deserialize(json);

	DESERIALIZE("local_position", mLocalPosition);
	DESERIALIZE("scale", mLocalScale);
	DESERIALIZE("rotation", mLocalRotation);
}
