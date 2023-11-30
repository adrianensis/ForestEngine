#include "Graphics/Renderer/MeshRenderer.hpp"

#include "Graphics/Material/TextureAnimation/TextureAnimationFrame.hpp"
#include "Graphics/GPU/GPUShader.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Mesh/MeshPrimitives.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Scene/Module.hpp"

void MeshRenderer::init(RendererData& data) 
{
    ComponentWithData::init(data);

    mMeshInstance = OwnerPtr<Mesh>::newObject();
    mMeshInstance->init(mComponentData.mMesh->mVertexCount, mComponentData.mMesh->mFacesCount);
    mMeshInstance->appendToBonesVertexIDsData(mComponentData.mMesh->mBonesVertexIDsData);
    mMeshInstance->appendToBonesVertexWeightsData(mComponentData.mMesh->mBonesVertexWeightsData);

    setColor(Vector4(0, 0, 0, 1));
    mRegeneratePositions = true;
    mRegenerateTextureCoords = true;
}

void MeshRenderer::onComponentAdded() 
{
    calculateRendererModelMatrix();
}

bool MeshRenderer::getIsWorldSpace() const
{
	return mGameObject->mTransform->mAffectedByProjection;
}

void MeshRenderer::calculateRendererModelMatrix()
{
    PROFILER_CPU()
    mRendererModelMatrix = mGameObject->mTransform->calculateModelMatrix();

    if(!mPositionOffset.eq(Vector3(0,0,0)))
    {
        PROFILER_BLOCK_CPU("Position offset");
        Matrix4 positionOffsetMatrix;
        positionOffsetMatrix.translation(mPositionOffset);

        positionOffsetMatrix.mul(mRendererModelMatrix);
        mRendererModelMatrix = positionOffsetMatrix;
    }

    IOcTreeElement::init(mRendererModelMatrix, getComponentData().mMesh->mMin, getComponentData().mMesh->mMax, getIsStatic());
    
    mRendererPositionOffsetDirty = false;
}

void MeshRenderer::preUpdate()
{
    mRendererModelMatrixDirty = mRendererPositionOffsetDirty || mGameObject->mTransform->getModelMatrixDirty();
}

void MeshRenderer::update()
{
	PROFILER_CPU()

    if(mRendererModelMatrixDirty)
    {
        calculateRendererModelMatrix();
    }

    bool regenerateVertices = !mComponentData.mIsInstanced;
    if(regenerateVertices)
    {
        if(mRegeneratePositions)
        {
            mMeshInstance->clearPositions();
            updatePositions();
            mRegeneratePositions = false;
        }
        if(mRegenerateTextureCoords)
        {
            mMeshInstance->clearTextureCoordinates();
            updateTextureCoords();
            mRegenerateTextureCoords = false;
        }
    }

    if(mRegenerateColor && mComponentData.mMaterial and mComponentData.mMaterial->getMaterialData().mUseVertexColor)
    {
        mMeshInstance->setColor(mColor);
        mRegenerateColor = false;
    }
}

void MeshRenderer::updatePositions()
{
	PROFILER_CPU()
    mMeshInstance->appendToPositions(mComponentData.mMesh->mPositions);

    if(mUseDepth)
    {
        FOR_RANGE(i, 0, mMeshInstance->mPositions.size())
        {
            Vector3 vertexPosition = mMeshInstance->mPositions[i];
            vertexPosition.z = mDepth;
            mMeshInstance->mPositions[i] = vertexPosition;
        }
    }
}

void MeshRenderer::updateTextureCoords()
{
	PROFILER_CPU()
    mMeshInstance->appendToTextureCoordinates(mComponentData.mMesh->mTextureCoordinates);

    updateTextureRegion();
    FOR_RANGE(i, 0, mMeshInstance->mVertexCount)
    {
        Vector2 vertexTexture = mComponentData.mMesh->mTextureCoordinates[i];
        Vector2 regionSize = mTextureRegion.getSize();
        Vector2 regionPosition = mTextureRegion.getLeftTopFront();

        Vector2 textureCoord(vertexTexture.x * regionSize.x + regionPosition.x, vertexTexture.y * regionSize.y + regionPosition.y);

        if (mInvertAxisX)
        {
            textureCoord.x = 1.0f - textureCoord.x;

            const TextureAnimation* currentTextureAnimation = getCurrentTextureAnimation();
            if (currentTextureAnimation)
            {
                textureCoord.x = textureCoord.x - (1.0f - (currentTextureAnimation->getNumberOfFrames() * regionSize.x));
            }
        }

        mMeshInstance->mTextureCoordinates[i] = textureCoord;
    }
}

void MeshRenderer::onDestroy() 
{

}

void MeshRenderer::setMaterial(Ptr<const Material> material)
{
    if(mComponentData.mMaterial != material)
    {
        mComponentData.mMaterial = material;
        if(mBatchRenderer)
        {
            mBatchRenderer->requestRegenerateBuffers();
        }
    }
}

void MeshRenderer::setColor(const Vector4& color)
{
    if(mColor != color)
    {
        mColor = color;
        mRegenerateColor = true;
        if(mBatchRenderer)
        {
            mBatchRenderer->requestRegenerateBuffers();
        }
    }
}

void MeshRenderer::updateTextureRegion()
{
	PROFILER_CPU()
	if (mComponentData.mMaterial.isValid())
	{
		const TextureAnimation* currentTextureAnimation = getCurrentTextureAnimation();

		if (currentTextureAnimation and !currentTextureAnimation->mFrames.empty())
		{
            mCurrentTextureAnimationUpdater.setTextureAnimation(*currentTextureAnimation);
			const TextureAnimationFrame& frame = mCurrentTextureAnimationUpdater.nextFrame();
			if(mCurrentTextureAnimationUpdater.getHasFrameChanged())
            {
                mTextureRegion.set(frame.mPosition, Vector2(frame.mWidth, frame.mHeight));
                mRegenerateTextureCoords = true;
            }
		}
	}
}

const TextureAnimation* MeshRenderer::getCurrentTextureAnimation() const
{
	const TextureAnimation* currentTextureAnimation = nullptr;
    if (mComponentData.mMaterial.isValid())
	{
        const auto& textureAnimationsMap = mComponentData.mMaterial->getMaterialData().mTextureAnimations;
		if (textureAnimationsMap.contains(mCurrentTextureAnimationKey))
		{
			currentTextureAnimation = &textureAnimationsMap.at(mCurrentTextureAnimationKey);
		}
	}

    return currentTextureAnimation;
}

void MeshRenderer::setDepth(i32 depth)
{
    if(mDepth != depth)
    {
        mDepth = depth;
        mRegeneratePositions = true;
    }
}

void MeshRenderer::setInvertAxisX(bool invertAxisX)
{
    if(mInvertAxisX != invertAxisX)
    {
        mInvertAxisX = invertAxisX;
        mRegenerateTextureCoords = true;
    }
}

void MeshRenderer::setTextureRegion(const Rectangle& textureRegion)
{
    mTextureRegion = textureRegion;
    mRegenerateTextureCoords = true;
}

void MeshRenderer::setPositionOffset(const Vector3& positionOffset)
{
    mPositionOffset = positionOffset;
    mRegeneratePositions = true;
    mRendererPositionOffsetDirty = true;
}

IMPLEMENT_SERIALIZATION(MeshRenderer)
{
	Component::serialize(json);

	std::string materialPath = "";

//	if(mMaterial->mTexture)
//	{
//		materialPath = mMaterial->mTexture->mPath;
//	}

	SERIALIZE("material", materialPath)
	// SERIALIZE("region", mTextureRegion)
	// SERIALIZE("depth", mDepth)
}

IMPLEMENT_DESERIALIZATION(MeshRenderer)
{
	std::string materialPath = "";
	DESERIALIZE("material", materialPath)

	//mMaterial = GET_SYSTEM(MaterialManager).loadMaterial(materialPath);

	// DESERIALIZE("region", mTextureRegion)
	// DESERIALIZE("depth", mComponentData.mDepth)
}