#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "GPU/InstanceRenderer/GPUInstanceRenderer.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Core/EntityComponent/ComponentPtr.hpp"
#include "Graphics/RenderPipeline/MeshRendererManager.hpp"

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

    template<class T> T_EXTENDS(T, RenderPass)
    void initRenderPass(const RenderPassData& renderPassData)
    {
        ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();

        mRenderPassMap.insert_or_assign(
            renderPassClassId,
            OwnerPtr<RenderPass>::moveCast(OwnerPtr<T>::newObject())
        );

        WeakPtr<T> renderPass = getRenderPass<T>();
        renderPass->init(mGPUInstanceRendererManager, renderPassData);
    }

    template<class T> T_EXTENDS(T, RenderPass)
    WeakPtr<T> getRenderPass()
    {
        ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        return WeakPtr<T>::cast(mRenderPassMap.at(renderPassClassId));
    }

    void initBuffers();

protected:
    std::unordered_map<ClassId, OwnerPtr<RenderPass>> mRenderPassMap;
    
    OwnerPtr<GPUInstanceRendererManager> mGPUInstanceRendererManager;
    MeshRendererManager mMeshRendererManager;
};
REGISTER_CLASS(RenderPipeline);
