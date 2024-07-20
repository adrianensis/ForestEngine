#include "Graphics/Renderer/MeshRenderer.hpp"

#include "Graphics/Material/TextureAnimation/TextureAnimationFrame.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/GPU/Mesh/MeshPrimitives.hpp"
#include "Graphics/GPU/Mesh/GPUMesh.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshRenderer.hpp"
#include "Scene/Module.hpp"

ClassId MeshRenderer::getSystemComponentId() const { return ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId(); }

void MeshRenderer::init(const RendererData& data) 
{
    mRendererData = data;
    mMaterialInstance = GET_SYSTEM(MaterialManager).createMaterialInstance(mRendererData.mMaterial);
    mMaterialInstance->setDirty();
}

void MeshRenderer::onComponentAdded() 
{
    calculateRendererModelMatrix();
}

void MeshRenderer::onDestroy() 
{
    GET_SYSTEM(MaterialManager).freeMaterialInstance(mMaterialInstance);
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
    //     //     mMaterialInstance.mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlock>().mTextureRegionLeftTop = frame.mPosition;
    //     //     mMaterialInstance.mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlock>().mTextureRegionSize = Vector2(frame.mWidth, frame.mHeight);
    //     // }
    // }
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