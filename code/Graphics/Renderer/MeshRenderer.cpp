#include "Graphics/Renderer/MeshRenderer.hpp"

#include "Graphics/Material/TextureAnimation/TextureAnimationFrame.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
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
    if(mMeshInstance->mBuffers.contains(GPUBuiltIn::VertexInput::mTextureCoord.mName))
    {
        ByteBuffer& bufferRefTexCoord = mMeshInstance->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoord.mName);
        bufferRefTexCoord.append(mRendererData.mMesh->mBuffers.at(GPUBuiltIn::VertexInput::mTextureCoord.mName));
    }

    mMaterialInstance = mRendererData.mMaterial->createMaterialInstance();

    GPUInstanceSlot slot = GET_SYSTEM(GPUSharedContext).requestInstanceSlot();
    mGPUInstanceSlot = slot;
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
    IOcTreeElement::init(mRendererModelMatrix, mRendererData.mMesh->mMin, mRendererData.mMesh->mMax, getIsStatic());
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
    GET_SYSTEM(GPUSharedContext).freeInstanceSlot(mGPUInstanceSlot);

    if(mBatchRenderer)
    {
        mBatchRenderer->requestRegenerateBuffers();
    }
}

void MeshRenderer::updateTextureRegion()
{
	PROFILER_CPU()

	const TextureAnimation* currentTextureAnimation = getCurrentTextureAnimation();
    if (currentTextureAnimation && !currentTextureAnimation->mFrames.empty())
    {
        mCurrentTextureAnimationUpdater.setTextureAnimation(*currentTextureAnimation);
        const TextureAnimationFrame& frame = mCurrentTextureAnimationUpdater.nextFrame();
        if(mCurrentTextureAnimationUpdater.getHasFrameChanged())
        {
            mMaterialInstance.mMaterialInstancedProperties.mTextureRegionLeftTop = frame.mPosition;
            mMaterialInstance.mMaterialInstancedProperties.mTextureRegionSize = Vector2(frame.mWidth, frame.mHeight);
        }
    }
}

const TextureAnimation* MeshRenderer::getCurrentTextureAnimation() const
{
	const TextureAnimation* currentTextureAnimation = nullptr;
    const auto& textureAnimationsMap = mRendererData.mMaterial->getMaterialData().mTextureAnimations;
    if (textureAnimationsMap.contains(mCurrentTextureAnimationKey))
    {
        currentTextureAnimation = &textureAnimationsMap.at(mCurrentTextureAnimationKey);
    }

    return currentTextureAnimation;
}

IMPLEMENT_SERIALIZATION(MeshRenderer)
{
	Component::serialize(json);
}

IMPLEMENT_DESERIALIZATION(MeshRenderer)
{

}