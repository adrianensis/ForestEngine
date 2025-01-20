#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshRenderer.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Core/ECS/ComponentHandler.hpp"

class RenderPipelineData
{
public:
    std::vector<TComponentHandler<PointLight>> mPointLights;
    TComponentHandler<DirectionalLight> mDirectionalLight;
};

class RenderPipeline: public EnablePtrToThis
{
public:
    virtual void init();
    void update();
    virtual void terminate();
    void addRenderer(TComponentHandler<MeshRenderer> renderer);
    void removeRenderer(TComponentHandler<MeshRenderer> renderer);
    virtual void render(RenderPipelineData& renderData);
    virtual void compile();

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
        renderPass->init(getPtrToThis<RenderPipeline>(), renderPassData);
        renderPass->compile();
    }

    template<class T> T_EXTENDS(T, RenderPass)
    WeakPtr<T> getRenderPass()
    {
        ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        return WeakPtr<T>::cast(mRenderPassMap.at(renderPassClassId));
    }

    void initBuffers();
    void setRendererMatrix(TComponentHandler<MeshRenderer> renderer);
    void processRenderer(TComponentHandler<MeshRenderer> renderer);

public:
    using InstancedMeshesMap = std::unordered_map<InstancedMeshData, OwnerPtr<InstancedMeshRenderer>, InstancedMeshData::InstancedMeshDataFunctor>;
private:
    std::unordered_map<ClassId, OwnerPtr<RenderPass>> mRenderPassMap;
    
    InstancedMeshesMap mInstancedMeshesMap;

    std::vector<Matrix4> mMatrices;
    SlotsManager mRenderInstancesSlotsManager;
	std::vector<TComponentHandler<MeshRenderer>> mRenderers;
    std::set<u32> mUsedSlots;
	std::vector<TComponentHandler<MeshRenderer>> mRenderersStatic;
    inline static const u32 mInitialInstances = 100;

public:
    CRGET(InstancedMeshesMap)
};
REGISTER_CLASS(RenderPipeline);
