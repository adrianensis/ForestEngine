#include "Graphics/MeshRenderer/MeshRenderer.hpp"

#include "Core/EntityComponent/Entity.hpp"
#include "GPU/GPUInstance.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"
#include "Scene/Transform.hpp"

void MeshRenderer::init(const GPURenderItemData& data) 
{
    mGPURenderItem = Core::OwnerPtr<GPURenderItem>::newObject();
    mGPURenderItem->init(data, false, GPUInstance::getInstance().mGPUShaderManager);
}

void MeshRenderer::onECComponentAdded() 
{
    mGPURenderItem->setIsStatic(CAST(GameObject,getOwnerEntity())->mIsStatic);
    calculateRendererModelMatrix();
}

void MeshRenderer::onDestroy() 
{
    mGPURenderItem->terminate(GPUInstance::getInstance().mGPUShaderManager);
    mGPURenderItem.invalidate();
}

void MeshRenderer::calculateRendererModelMatrix()
{
    PROFILER_CPU()
    Maths::Matrix4 rendererModelMatrix = getOwnerGameObject()->getSystemsDI().getSystem<EC::EntityComponentManager>()->getFirstComponent<Transform>(getOwnerEntity())->calculateModelMatrix();
    rendererModelMatrix.mul(mGPURenderItem->getGPURenderItemData().mMeshInstanceMatrix);
    // IOcTreeElement::init(mRendererModelMatrix, mRendererData.mMesh->mMin, mRendererData.mMesh->mMax, getIsStatic());
    mGPURenderItem->setRendererModelMatrix(rendererModelMatrix);
    mGPURenderItem->setUpdateMatrix(true);
}

void MeshRenderer::update()
{
	PROFILER_CPU()

    if(! CAST(GameObject, getOwnerEntity())->mIsStatic)
    {
        if(getOwnerGameObject()->getSystemsDI().getSystem<EC::EntityComponentManager>()->getFirstComponent<Transform>(getOwnerEntity())->getModelMatrixDirty())
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
    //     //     mGPUShaderPropertiesInstance.mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlock>().mTextureRegionSize = Maths::Vector2(frame.mWidth, frame.mHeight);
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