#pragma once

#include "GPU/Core/GPUContext.hpp"
// TODO: fix .h extension, should be .hpp
#include "GPU/Descriptors/GPUDescriptorManager.hpp"
#include "GPU/RenderPass/GPURenderPass.h"
#include "GPU/RenderItem/GPURenderItem.hpp"

class GPURenderGraph
{
public:
    void init(GPUContext* gpuContext, GPURenderPassSubsystems& gpuRenderPassSubsystems);
    void update();
    void terminate();
    void render();
    void addRenderer(GPURenderItem* renderItem);
    void removeRenderer(GPURenderItem* renderItem);
    void onResize();

    template<class T>
    requires std::derived_from<T, GPURenderPass>
    void initRenderPass(const GPURenderPassData& renderPassData)
    {
        GPURenderPassID renderPassClassId = GPURenderPass::getID<T>();

        mRenderPassMap.insert_or_assign(
            renderPassClassId,
            new T()
        );

        T* renderPass = getRenderPass<T>();
        renderPass->init(mGPUContext,
            renderPassData,
            mGPURenderPassSubsystems);

        mRenderPassesArray.push_back(renderPass);
    }

    template<class T>
    requires std::derived_from<T, GPURenderPass>
    T* getRenderPass()
    {
        GPURenderPassID renderPassClassId = GPURenderPass::getID<T>();
        return static_cast<T*>(mRenderPassMap.at(renderPassClassId));
    }

    void initBuffers();

private:
    GPUContext* mGPUContext = nullptr;
    std::unordered_map<GPURenderPassID, GPURenderPass*> mRenderPassMap;
    std::vector<GPURenderPass*> mRenderPassesArray;
    GPURenderPassSubsystems mGPURenderPassSubsystems;
    GPURenderPass* mRenderPassResolve = nullptr;
    // GPUDescriptorSet* mGPUDescriptorSet = nullptr;
    GPUImage mColorBufferImage;
    GPUImage mDepthBufferImage;
};

