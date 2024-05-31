#include "Graphics/Renderer/MeshRenderer.hpp"

#include "Graphics/Material/TextureAnimation/TextureAnimationFrame.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUState.hpp"
#include "Graphics/RenderState.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Mesh/MeshPrimitives.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Renderer/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/SkeletalAnimation/SkeletalAnimationManager.hpp"
#include "Scene/Module.hpp"

ClassId MeshRenderer::getSystemComponentId() const { return ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId(); }

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

    mRenderInstanceSlot = GET_SYSTEM(RenderState).requestRenderInstanceSlot();
    mMaterialInstance = GET_SYSTEM(MaterialManager).createMaterialInstance(mRendererData.mMaterial);
    mMaterialInstanceSlot = GET_SYSTEM(MaterialManager).requestMaterialInstanceSlot(mRendererData.mMaterial);
}

void MeshRenderer::onComponentAdded() 
{
    calculateRendererModelMatrix();
}

void MeshRenderer::onDestroy() 
{
    GET_SYSTEM(RenderState).freeRenderInstanceSlot(mRenderInstanceSlot);
    GET_SYSTEM(MaterialManager).freeMaterialInstanceSlot(getRendererData().mMaterial, mMaterialInstanceSlot);
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

void MeshRenderer::updateTextureRegion()
{
	PROFILER_CPU()

	const TextureAnimation* currentTextureAnimation = getCurrentTextureAnimation();
    if (currentTextureAnimation && !currentTextureAnimation->mFrames.empty())
    {
        mCurrentTextureAnimationUpdater.setTextureAnimation(*currentTextureAnimation);
        const TextureAnimationFrame& frame = mCurrentTextureAnimationUpdater.nextFrame();
        // if(mCurrentTextureAnimationUpdater.getHasFrameChanged())
        // {
        //     mMaterialInstance.mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlock>().mTextureRegionLeftTop = frame.mPosition;
        //     mMaterialInstance.mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlock>().mTextureRegionSize = Vector2(frame.mWidth, frame.mHeight);
        // }
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