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

void MeshRenderer::init(const RendererData& data) 
{
    mRendererData = data;

    mMeshInstance = OwnerPtr<Mesh>::newObject();
    mMeshInstance->init(mRendererData.mMesh->mVertexCount, mRendererData.mMesh->mIndicesCount, mRendererData.mMesh->mGPUVertexInputBuffers);
    if(mRendererData.mMaterial->getMaterialData().mIsSkinned)
    {
        mMeshInstance->mBuffers.at(GPUBuiltIn::VertexInput::mBonesIDs.mName).append(mRendererData.mMesh->mBuffers.at(GPUBuiltIn::VertexInput::mBonesIDs.mName));
        mMeshInstance->mBuffers.at(GPUBuiltIn::VertexInput::mBonesWeights.mName).append(mRendererData.mMesh->mBuffers.at(GPUBuiltIn::VertexInput::mBonesWeights.mName));
    }

    ByteBuffer& bufferRefPosition = mMeshInstance->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName);
    bufferRefPosition.append(mRendererData.mMesh->mBuffers.at(GPUBuiltIn::VertexInput::mPosition.mName));
    ByteBuffer& bufferRefTexCoord = mMeshInstance->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoord.mName);
    bufferRefTexCoord.append(mRendererData.mMesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoord.mName));

    mMaterialInstance = mRendererData.mMaterial->createMaterialInstance();
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

    IOcTreeElement::init(mRendererModelMatrix, mRendererData.mMesh->mMin, mRendererData.mMesh->mMax, getIsStatic());
    
    mRendererPositionOffsetDirty = false;
}

void MeshRenderer::update()
{
	PROFILER_CPU()

    if(!isStatic())
    {
        calculateRendererModelMatrix();
    }

    updateTextureRegion();
}

void MeshRenderer::onDestroy() 
{

}

void MeshRenderer::setMaterial(Ptr<const Material> material)
{
    if(mRendererData.mMaterial != material)
    {
        mRendererData.mMaterial = material;
        if(mBatchRenderer)
        {
            mBatchRenderer->requestRegenerateBuffers();
        }
    }
}

void MeshRenderer::updateTextureRegion()
{
	PROFILER_CPU()
	if (mRendererData.mMaterial.isValid())
	{
		const TextureAnimation* currentTextureAnimation = getCurrentTextureAnimation();

		if (currentTextureAnimation && !currentTextureAnimation->mFrames.empty())
		{
            mCurrentTextureAnimationUpdater.setTextureAnimation(*currentTextureAnimation);
			const TextureAnimationFrame& frame = mCurrentTextureAnimationUpdater.nextFrame();
			if(mCurrentTextureAnimationUpdater.getHasFrameChanged())
            {
                setTextureRegion(Rectangle(frame.mPosition, Vector2(frame.mWidth, frame.mHeight)));
            }
		}
	}
}

const TextureAnimation* MeshRenderer::getCurrentTextureAnimation() const
{
	const TextureAnimation* currentTextureAnimation = nullptr;
    if (mRendererData.mMaterial.isValid())
	{
        const auto& textureAnimationsMap = mRendererData.mMaterial->getMaterialData().mTextureAnimations;
		if (textureAnimationsMap.contains(mCurrentTextureAnimationKey))
		{
			currentTextureAnimation = &textureAnimationsMap.at(mCurrentTextureAnimationKey);
		}
	}

    return currentTextureAnimation;
}

void MeshRenderer::setDepth(i32 depth)
{
    mMaterialInstance.mMaterialInstancedProperties.mDepth = depth;
}

void MeshRenderer::setTextureRegion(const Rectangle& textureRegion)
{
    mMaterialInstance.mMaterialInstancedProperties.mTextureRegionTopLeft = textureRegion.getLeftTopFront();
    mMaterialInstance.mMaterialInstancedProperties.mTextureRegionSize = textureRegion.getSize();
}

void MeshRenderer::setPositionOffset(const Vector3& positionOffset)
{
    mPositionOffset = positionOffset;
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
	// DESERIALIZE("depth", mRendererData.mDepth)
}