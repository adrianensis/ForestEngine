#pragma once

#include "GPU/Core/GPUContext.hpp"

class GPURenderGraphData
{
public:
    // std::vector<TComponentPtr<PointLight>> mPointLights;
    // TComponentPtr<DirectionalLight> mDirectionalLight;
};

class GPURenderGraph
{
public:
    void init(Ptr<GPUContext> gpuContext);
    void update();
    virtual void terminate();
    virtual void render(GPURenderGraphData& renderData);
    void onResize();

protected:
    // template<class T> T_EXTENDS(T, RenderPass)
    // void initRenderPass(const RenderPassData& renderPassData)
    // {
    //     ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
    //     mRenderPassMap.insert_or_assign(
    //         renderPassClassId,
    //         OwnerPtr<RenderPass>::moveCast(OwnerPtr<T>::newObject())
    //     );

    //     WeakPtr<T> renderPass = getRenderPass<T>();
    //     renderPass->init(this, renderPassData);
    // }

    // template<class T> T_EXTENDS(T, RenderPass)
    // WeakPtr<T> getRenderPass()
    // {
    //     ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
    //     return WeakPtr<T>::cast(mRenderPassMap.at(renderPassClassId));
    // }

protected:
    Ptr<GPUContext> mGPUContext;
    // std::unordered_map<ClassId, OwnerPtr<RenderPass>> mRenderPassMap;
};
REGISTER_CLASS(GPURenderGraph);
