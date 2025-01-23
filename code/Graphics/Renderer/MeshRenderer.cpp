#include "Graphics/Renderer/MeshRenderer.hpp"

#include "Graphics/TextureAnimation/TextureAnimationFrame.hpp"
#include "GPU/GPUInstance.hpp"
#include "GPU/Image/GPUTexture.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "GPU/Mesh/GPUMeshFactory.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "Graphics/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshRenderer.hpp"
#include "Scene/Module.hpp"

ClassId MeshRenderer::getComponentTypeId() const { return ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId(); }

void MeshRenderer::init(const RendererData& data) 
{
    mRendererData = data;
    mShaderInstance = GET_SYSTEM(ShaderManager).createShaderInstance(mRendererData.mShader);
    mShaderInstance->setDirty();
}

void MeshRenderer::onComponentAdded() 
{
    calculateRendererModelMatrix();
}

void MeshRenderer::onDestroy() 
{
    GET_SYSTEM(ShaderManager).freeShaderInstance(mShaderInstance);
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
    //     //     mShaderInstance.mShaderPropertiesBlockBuffer.get<ShaderPropertiesBlock>().mTextureRegionLeftTop = frame.mPosition;
    //     //     mShaderInstance.mShaderPropertiesBlockBuffer.get<ShaderPropertiesBlock>().mTextureRegionSize = Vector2(frame.mWidth, frame.mHeight);
    //     // }
    // }
}

const TextureAnimation* MeshRenderer::getCurrentTextureAnimation() const
{
	const TextureAnimation* currentTextureAnimation = nullptr;
    // const auto& textureAnimationsMap = mRendererData.mShader->getShaderData().mTextureAnimations;
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