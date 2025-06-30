#include "Graphics/MeshRenderer/MeshRenderer.hpp"

#include "GPU/Image/GPUTexture.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Scene/Transform.hpp"

ClassId MeshRenderer::getComponentTypeId() const { return Core::ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId(); }

void MeshRenderer::init(const GPURenderItemData& data) 
{
    mGPURenderItem = OwnerPtr<GPURenderItem>::newObject();
    mGPURenderItem->init(data, false);
}

void MeshRenderer::onComponentAdded() 
{
    mGPURenderItem->setIsStatic(isStatic());
    calculateRendererModelMatrix();
}

void MeshRenderer::onDestroy() 
{
    mGPURenderItem->terminate();
    mGPURenderItem.invalidate();
}

void MeshRenderer::calculateRendererModelMatrix()
{
    PROFILER_CPU()
    Matrix4 rendererModelMatrix = EC.getFirstComponent<Transform>(getOwnerEntity())->calculateModelMatrix();
    rendererModelMatrix.mul(mGPURenderItem->getGPURenderItemData().mMeshInstanceMatrix);
    // IOcTreeElement::init(mRendererModelMatrix, mRendererData.mMesh->mMin, mRendererData.mMesh->mMax, getIsStatic());
    mGPURenderItem->setRendererModelMatrix(rendererModelMatrix);
    mGPURenderItem->setUpdateMatrix(true);
}

void MeshRenderer::update()
{
	PROFILER_CPU()

    if(!isStatic())
    {
        if(EC.getFirstComponent<Transform>(getOwnerEntity())->getModelMatrixDirty())
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