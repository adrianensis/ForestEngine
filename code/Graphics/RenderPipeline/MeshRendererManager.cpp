#include "Graphics/RenderPipeline/MeshRendererManager.hpp"
#include "GPU/GPUInstance.hpp"
#include "Scene/Module.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"

void MeshRendererManager::init()
{
    PROFILER_CPU()

    mRenderInstancesSlotsManager.init(mInitialInstances * 100);
    mRenderers.resize(mRenderInstancesSlotsManager.getSize());
    mRenderersStatic.resize(mRenderInstancesSlotsManager.getSize());
    mMatrices.resize(mRenderInstancesSlotsManager.getSize());
}

void MeshRendererManager::update()
{
	PROFILER_CPU()
    PROFILER_CPU_NAMED(updateRenderers);
    if(!mUsedSlots.empty())
    {
        FOR_RANGE(i, *mUsedSlots.begin(), (*mUsedSlots.rbegin())+1)
        {
            TComponentPtr<MeshRenderer> renderer = mRenderers[i];
            if(renderer.isValid())
            {
                processRenderer(renderer);
            }
        }
    }
}

void MeshRendererManager::processRenderer(TComponentPtr<MeshRenderer> renderer)
{
	PROFILER_CPU()
    if(!renderer->isStatic())
    {
        renderer->update();
        setRendererMatrix(renderer);
    }
}

void MeshRendererManager::terminate()
{
    mRenderInstancesSlotsManager.reset();
}

void MeshRendererManager::addRenderer(TComponentPtr<MeshRenderer> renderer)
{
    PROFILER_CPU()
    if(mRenderInstancesSlotsManager.isEmpty())
    {
        mRenderInstancesSlotsManager.increaseSize(mInitialInstances);
        mRenderersStatic.resize(mRenderInstancesSlotsManager.getSize());
        mRenderers.resize(mRenderInstancesSlotsManager.getSize());
        mMatrices.resize(mRenderInstancesSlotsManager.getSize());
    }

    renderer->getGPURenderItem()->setRenderSlot(mRenderInstancesSlotsManager.requestSlot());
    if(renderer->isStatic())
    {
        setRendererMatrix(renderer);
        mRenderersStatic.at(renderer->getGPURenderItem()->getRenderSlot().getSlot()) = renderer;
    }
    else
    {
        mUsedSlots.insert(renderer->getGPURenderItem()->getRenderSlot().getSlot());
        mRenderers.at(renderer->getGPURenderItem()->getRenderSlot().getSlot()) = renderer;
    }
}

void MeshRendererManager::removeRenderer(TComponentPtr<MeshRenderer> renderer)
{
    PROFILER_CPU()
    if(renderer->isStatic())
    {
        mRenderersStatic.at(renderer->getGPURenderItem()->getRenderSlot().getSlot()).reset();
    }
    else
    {
        mUsedSlots.erase(renderer->getGPURenderItem()->getRenderSlot().getSlot());
        mRenderers.at(renderer->getGPURenderItem()->getRenderSlot().getSlot()).reset();
    }

    mRenderInstancesSlotsManager.freeSlot(renderer->getGPURenderItem()->getRenderSlot());

    // FOR_LIST(it, renderer->getGPURenderItemData().mRenderPassIDs)
    // {
    //     if(mRenderPassMap.contains(*it))
    //     {
    //         mRenderPassMap.at(*it)->removeRenderer(renderer);
    //     }
    // }
}

void MeshRendererManager::setRendererMatrix(TComponentPtr<MeshRenderer> renderer)
{
    PROFILER_CPU()
    if(renderer->getUpdateMatrix())
    {
        const Matrix4& rendererModelMatrix = renderer->getGPURenderItem()->getRendererModelMatrix();
        CHECK_MSG(mRenderInstancesSlotsManager.checkSlot(renderer->getGPURenderItem()->getRenderSlot()), "Invalid slot!");
        mMatrices.at(renderer->getGPURenderItem()->getRenderSlot().getSlot()) = rendererModelMatrix;
        renderer->setUpdateMatrix(false);
    }
}