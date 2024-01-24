#include "Scene/Transform.hpp"
#include "Scene/GameObject.hpp"

void Transform::init() 
{
	mTranslationMatrix.identity();
	mRotationMatrix.identity();
	mScaleMatrix.identity();

	mLocalPosition = Vector3(0.0f, 0.0f, 0.0f);
	mRotation = Vector3(0.0f, 0.0f, 0.0f);
	mScale = Vector3(1.0f, 1.0f, 1.0f);

	notifyModelMatrixDirty();
}

void Transform::onDestroy()
{
    if(mParent)
    {
        mParent->removeChild(getPtrToThis());
    }
}

Vector3 Transform::getWorldPosition() const
{
	Vector3 worldPosition = mLocalPosition;
	if (mParent)
	{
        const Matrix4& parentModelMatrix = mParent->calculateModelMatrix();
        worldPosition = parentModelMatrix.mulVector(Vector4(worldPosition, 1.0f));
	}

	return worldPosition;
}

Vector3 Transform::getWorldScale() const
{
	Vector3 worldScale = mScale;
    if (mParent)
    {
        const Matrix4& parentModelMatrix = mParent->calculateModelMatrix();
        worldScale = parentModelMatrix.mulVector(Vector4(worldScale, 1.0f));
    }

	return worldScale;
}

Vector3 Transform::getWorldRotation() const
{
	Vector3 worldRotation = mRotation;
	if (mParent)
	{
        const Matrix4& parentModelMatrix = mParent->calculateModelMatrix();
        worldRotation = parentModelMatrix.mulVector(Vector4(worldRotation, 1.0f));
	}

	return worldRotation;
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
    if(mModelMatrixDirty)
    {
	    mTranslationMatrix.translation(mLocalPosition);
    }
	return mTranslationMatrix;
}

const Matrix4& Transform::getRotationMatrix() const
{
    if(mModelMatrixDirty)
    {
	    mRotationMatrix.rotation(mRotation);
    }
	return mRotationMatrix;
}

const Matrix4& Transform::getScaleMatrix() const
{
    if(mModelMatrixDirty)
    {
	    mScaleMatrix.scale(mScale);
    }
	return mScaleMatrix;
}

const Matrix4& Transform::calculateModelMatrix() const
{
    PROFILER_CPU()

    if(mModelMatrixDirty)
    {
        Matrix4 translationMatrix(getTranslationMatrix());
        Matrix4 rotationMatrix(getRotationMatrix());
        Matrix4 scaleMatrix(getScaleMatrix());

        rotationMatrix.mul(scaleMatrix);
        translationMatrix.mul(rotationMatrix);
        mModelMatrix.init(translationMatrix);

        translationMatrix.init(getTranslationMatrix());
        rotationMatrix.init(getRotationMatrix());

        translationMatrix.mul(rotationMatrix);
        mModelMatrixNoScale.init(translationMatrix);

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
    mModelMatrixDirty = true;
    FOR_MAP(it, mChildren)
    {
        it->second->notifyModelMatrixDirty();
    }
}

void Transform::translate(const Vector3& vector)
{
    setPosition(mLocalPosition.add(vector));
}

void Transform::rotate(const Vector3& vector)
{
    setRotation(mRotation.add(vector));
}

void Transform::setPosition(const Vector3& vec)
{
    mLocalPosition = vec;
    notifyModelMatrixDirty();
}

void Transform::setRotation(const Vector3& vec)
{
    mRotation = vec;
    notifyModelMatrixDirty();
}

void Transform::setScale(const Vector3& vec)
{
    mScale = vec;
    notifyModelMatrixDirty();
}

void Transform::addChild(Ptr<Transform> child)
{
    child->mParent = getPtrToThis();
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
