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

    template<class T> T_EXTENDS(T, GPURenderPass)
    void initRenderPass(const GPURenderPassData& renderPassData)
    {
        Core::ClassId renderPassClassId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();

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

    template<class T> T_EXTENDS(T, GPURenderPass)
    T* getRenderPass()
    {
        Core::ClassId renderPassClassId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        return static_cast<T*>(mRenderPassMap.at(renderPassClassId));
    }

    void initBuffers();

private:
    GPUContext* mGPUContext = nullptr;
    std::unordered_map<Core::ClassId, GPURenderPass*> mRenderPassMap;
    std::vector<GPURenderPass*> mRenderPassesArray;
    GPURenderPassSubsystems mGPURenderPassSubsystems;
    GPURenderPass* mRenderPassResolve = nullptr;
    // GPUDescriptorSet* mGPUDescriptorSet = nullptr;
    GPUImage mColorBufferImage;
    GPUImage mDepthBufferImage;
};

