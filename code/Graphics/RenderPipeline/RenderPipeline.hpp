#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Light/Light.hpp"
#include "GPU/InstanceRenderer/GPUInstanceRenderer.hpp"
#include "GPU/RenderPass/GPURenderPass.h"
#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Core/EntityComponent/ComponentPtr.hpp"
#include "Graphics/RenderPipeline/MeshRendererManager.hpp"
#include "GPU/GPUInstance.hpp"

class RenderPipelineData
{
public:
    std::vector<TComponentPtr<PointLight>> mPointLights;
    TComponentPtr<DirectionalLight> mDirectionalLight;
};

class RenderPipeline
{
public:
    virtual void init();
    virtual ~RenderPipeline() = default;
    void update();
    virtual void terminate();
    void addRenderer(TComponentPtr<MeshRenderer> renderer);
    void removeRenderer(TComponentPtr<MeshRenderer> renderer);
    virtual void render(RenderPipelineData& renderData);
    virtual void compile();
    void onResize();

protected:
    void updateLights(RenderPipelineData& renderData);

    template<class T> T_EXTENDS(T, GPURenderPass)
    void initRenderPass(const GPURenderPassData& renderPassData)
    {
        ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();

        mRenderPassMap.insert_or_assign(
            renderPassClassId,
            OwnerPtr<GPURenderPass>::moveCast(OwnerPtr<T>::newObject())
        );

        WeakPtr<T> renderPass = getRenderPass<T>();
        renderPass->init(GET_SYSTEM(GPUInstance).mGPUContext, mGPUInstanceRendererManager, renderPassData);
    }

    template<class T> T_EXTENDS(T, GPURenderPass)
    WeakPtr<T> getRenderPass()
    {
        ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        return WeakPtr<T>::cast(mRenderPassMap.at(renderPassClassId));
    }

    void initBuffers();

protected:
    std::unordered_map<ClassId, OwnerPtr<GPURenderPass>> mRenderPassMap;
    
    OwnerPtr<GPUInstanceRendererManager> mGPUInstanceRendererManager;
    MeshRendererManager mMeshRendererManager;
};
REGISTER_CLASS(RenderPipeline);
