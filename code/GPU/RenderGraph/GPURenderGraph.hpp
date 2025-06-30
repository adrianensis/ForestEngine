#pragma once

#include "GPU/Core/GPUContext.hpp"
// TODO: fix .h extension, should be .hpp
#include "GPU/RenderPass/GPURenderPass.h"
#include "GPU/RenderItem/GPURenderItem.hpp"
#include "GPU/GPUInstance.hpp"

class GPURenderGraphData
{
public:
    // std::vector<TComponentPtr<PointLight>> mPointLights;
    // TComponentPtr<DirectionalLight> mDirectionalLight;
};

class GPURenderGraph
{
public:
    void init(Ptr<GPUContext> gpuContext, WeakPtr<GPUInstanceRendererManager> gpuInstanceRendererManager);
    void update();
    void terminate();
    void render(GPURenderGraphData& renderData);
    void addRenderer(WeakPtr<GPURenderItem> renderItem);
    void removeRenderer(WeakPtr<GPURenderItem> renderItem);
    void onResize();

    void updateLights(GPURenderGraphData& renderData);

    template<class T> T_EXTENDS(T, GPURenderPass)
    void initRenderPass(const GPURenderPassData& renderPassData)
    {
        ClassId renderPassClassId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();

        mRenderPassMap.insert_or_assign(
            renderPassClassId,
            OwnerPtr<GPURenderPass>::moveCast(OwnerPtr<T>::newObject())
        );

        GPURenderPassOutputData renderPassOutputData;
        renderPassOutputData.mColorGPUImage = &vulkanColorImage;

        WeakPtr<T> renderPass = getRenderPass<T>();
        renderPass->init(GET_SYSTEM(GPUInstance).mGPUContext, mGPUInstanceRendererManager, renderPassData, renderPassOutputData);

        mRenderPassesArray.push_back(renderPass);
    }

    template<class T> T_EXTENDS(T, GPURenderPass)
    WeakPtr<T> getRenderPass()
    {
        ClassId renderPassClassId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        return WeakPtr<T>::cast(mRenderPassMap.at(renderPassClassId));
    }

    void initBuffers();

private:
    Ptr<GPUContext> mGPUContext;
    std::unordered_map<ClassId, OwnerPtr<GPURenderPass>> mRenderPassMap;
    std::vector<WeakPtr<GPURenderPass>> mRenderPassesArray;
    WeakPtr<GPUInstanceRendererManager> mGPUInstanceRendererManager;
    OwnerPtr<GPURenderPass> mRenderPassResolve;
    GPUImage vulkanColorImage;
};
REGISTER_CLASS(GPURenderGraph);
