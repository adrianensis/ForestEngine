#pragma once

#include "GPU/Core/GPUContext.hpp"
// TODO: fix .h extension, should be .hpp
#include "GPU/RenderPass/GPURenderPass.h"
#include "GPU/RenderItem/GPURenderItem.hpp"
#include "GPU/GPUInstance.hpp"

class GPURenderGraphData
{
public:
    // std::vector<EC::TComponentPtr<PointLight>> mPointLights;
    // EC::TComponentPtr<DirectionalLight> mDirectionalLight;
};

class GPURenderGraph
{
public:
    void init(Core::Ptr<GPUContext> gpuContext, Core::WeakPtr<GPUInstanceRendererManager> gpuInstanceRendererManager);
    void update();
    void terminate();
    void render(GPURenderGraphData& renderData);
    void addRenderer(Core::WeakPtr<GPURenderItem> renderItem);
    void removeRenderer(Core::WeakPtr<GPURenderItem> renderItem);
    void onResize();

    void updateLights(GPURenderGraphData& renderData);

    template<class T> T_EXTENDS(T, GPURenderPass)
    void initRenderPass(const GPURenderPassData& renderPassData)
    {
        Core::ClassId renderPassClassId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();

        mRenderPassMap.insert_or_assign(
            renderPassClassId,
            Core::OwnerPtr<GPURenderPass>::moveCast(Core::OwnerPtr<T>::newObject())
        );

        GPURenderPassOutputData renderPassOutputData;
        renderPassOutputData.mColorGPUImage = &vulkanColorImage;

        Core::WeakPtr<T> renderPass = getRenderPass<T>();
        renderPass->init(GET_SYSTEM(GPUInstance).mGPUContext, mGPUInstanceRendererManager, renderPassData, renderPassOutputData);

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
    Core::Ptr<GPUContext> mGPUContext;
    std::unordered_map<Core::ClassId, Core::OwnerPtr<GPURenderPass>> mRenderPassMap;
    std::vector<Core::WeakPtr<GPURenderPass>> mRenderPassesArray;
    Core::WeakPtr<GPUInstanceRendererManager> mGPUInstanceRendererManager;
    Core::OwnerPtr<GPURenderPass> mRenderPassResolve;
    GPUImage vulkanColorImage;
};
REGISTER_CLASS(GPURenderGraph);
