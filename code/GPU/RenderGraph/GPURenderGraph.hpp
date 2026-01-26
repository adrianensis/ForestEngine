#pragma once

#include "GPU/Core/GPUContext.hpp"
// TODO: fix .h extension, should be .hpp
#include "GPU/RenderPass/GPURenderPass.h"
#include "GPU/RenderItem/GPURenderItem.hpp"

class GPURenderGraph
{
public:
    void init(GPUContext* gpuContext, Core::WeakPtr<GPUInstanceRendererManager> gpuInstanceRendererManager, Core::WeakPtr<GPUUniformBuffersContainer> globalGPUUniformBuffersContainer,
        GPUSkeletalAnimationManager* gpuSkeletalAnimationManager, GPUShaderManager* gpuShaderManager);
    void update();
    void terminate();
    void render();
    void addRenderer(Core::WeakPtr<GPURenderItem> renderItem);
    void removeRenderer(Core::WeakPtr<GPURenderItem> renderItem);
    void onResize();

    template<class T> T_EXTENDS(T, GPURenderPass)
    void initRenderPass(const GPURenderPassData& renderPassData, GPUSkeletalAnimationManager* gpuSkeletalAnimationManager, GPUShaderManager* gpuShaderManager)
    {
        Core::ClassId renderPassClassId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();

        mRenderPassMap.insert_or_assign(
            renderPassClassId,
            Core::OwnerPtr<GPURenderPass>::moveCast(Core::OwnerPtr<T>::newObject())
        );

        GPURenderPassOutputData renderPassOutputData;
        renderPassOutputData.mColorGPUImage = &vulkanColorImage;

        Core::WeakPtr<T> renderPass = getRenderPass<T>();
        renderPass->init(mGPUContext, mGPUInstanceRendererManager, mGlobalGPUUniformBuffersContainer, renderPassData, renderPassOutputData, gpuSkeletalAnimationManager, gpuShaderManager);

        mRenderPassesArray.push_back(renderPass);
    }

    template<class T> T_EXTENDS(T, GPURenderPass)
    Core::WeakPtr<T> getRenderPass()
    {
        Core::ClassId renderPassClassId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        return Core::WeakPtr<T>::cast(mRenderPassMap.at(renderPassClassId));
    }

    void initBuffers();

private:
    GPUContext* mGPUContext = nullptr;
    std::unordered_map<Core::ClassId, Core::OwnerPtr<GPURenderPass>> mRenderPassMap;
    std::vector<Core::WeakPtr<GPURenderPass>> mRenderPassesArray;
    Core::WeakPtr<GPUInstanceRendererManager> mGPUInstanceRendererManager;
    Core::WeakPtr<GPUUniformBuffersContainer> mGlobalGPUUniformBuffersContainer;
    Core::OwnerPtr<GPURenderPass> mRenderPassResolve;
    GPUImage vulkanColorImage;
};

