#include "Graphics/Renderer/MeshRenderer.hpp"

#include "GPU/Image/GPUTexture.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "Core/EntityComponent/EntityPtr.hpp"
#include "Scene/Transform.hpp"

ClassId MeshRenderer::getComponentTypeId() const { return ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId(); }

void MeshRenderer::init(const RendererData& data) 
{
    mRendererData = data;
    mGPUShaderPropertiesInstance = GET_SYSTEM(GPUShaderManager).createGPUShaderPropertiesInstance(mRendererData.mShader);
    mGPUShaderPropertiesInstance->setDirty();
}

void MeshRenderer::onComponentAdded() 
{
    calculateRendererModelMatrix();
}

void MeshRenderer::onDestroy() 
{
    GET_SYSTEM(GPUShaderManager).freeGPUShaderPropertiesInstance(mGPUShaderPropertiesInstance);
    mRenderSlot.reset();
    mInstanceSlot.reset();
}

void MeshRenderer::calculateRendererModelMatrix()
{
    PROFILER_CPU()
    mRendererModelMatrix = getOwnerEntity()->getFirstComponent<Transform>()->calculateModelMatrix();
    mRendererModelMatrix.mul(mRendererData.mMeshInstanceMatrix);
    // IOcTreeElement::init(mRendererModelMatrix, mRendererData.mMesh->mMin, mRendererData.mMesh->mMax, getIsStatic());
    setUpdateMatrix(true);
}

void MeshRenderer::update()
{
	PROFILER_CPU()

    if(!isStatic())
    {
        if(getOwnerEntity()->getFirstComponent<Transform>()->getModelMatrixDirty())
        {
            calculateRendererModelMatrix();
        }
    }

    updateTextureRegion();
}

void MeshRenderer::updateTextureRegion()
{
	// PROFILER_CPU()

	// const TextureAnimation* currentTextureAnimation = getCurrentTextureAnimation();
    // if (currentTextureAnimation && !currentTextureAnimation->mFrames.empty())
    // {
    //     mCurrentTextureAnimationUpdater.setTextureAnimation(*currentTextureAnimation);
    //     const TextureAnimationFrame& frame = mCurrentTextureAnimationUpdater.nextFrame();
    //     // if(mCurrentTextureAnimationUpdater.getHasFrameChanged())
    //     // {
    //     //     mGPUShaderPropertiesInstance.mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlock>().mTextureRegionLeftTop = frame.mPosition;
    //     //     mGPUShaderPropertiesInstance.mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlock>().mTextureRegionSize = Vector2(frame.mWidth, frame.mHeight);
    //     // }
    // }
}

const TextureAnimation* MeshRenderer::getCurrentTextureAnimation() const
{
	const TextureAnimation* currentTextureAnimation = nullptr;
    // const auto& textureAnimationsMap = mRendererData.mShader->getGPUShaderData().mTextureAnimations;
    // if (textureAnimationsMap.contains(mCurrentTextureAnimationKey))
    // {
    //     currentTextureAnimation = &textureAnimationsMap.at(mCurrentTextureAnimationKey);
    // }

    return currentTextureAnimation;
}

IMPLEMENT_SERIALIZATION(MeshRenderer)
{
	Component::serialize(json);
}

IMPLEMENT_DESERIALIZATION(MeshRenderer)
{

}