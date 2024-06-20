#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Renderer/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"

class RenderPipelineData
{
public:
    std::vector<Ptr<PointLight>> mPointLights;
    Ptr<DirectionalLight> mDirectionalLight;
};

class RenderPipeline: public ObjectBase
{
public:
    virtual void init();
    virtual void update();
    virtual void terminate();
    void addRenderer(Ptr<MeshRenderer> renderer);
    void removeRenderer(Ptr<MeshRenderer> renderer);
    virtual void render(RenderPipelineData& renderData);

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

        Ptr<T> renderPass = getRenderPass<T>();
        renderPass->init(getPtrToThis<RenderPipeline>(), renderPassData);
    }

    template<class T> T_EXTENDS(T, RenderPass)
    Ptr<T> getRenderPass()
    {
        ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        return Ptr<T>::cast(mRenderPassMap.at(renderPassClassId));
    }

    void initBuffers();
    void setRendererMatrix(Ptr<MeshRenderer> renderer);

public:
    using BatchMap = std::unordered_map<BatchData, OwnerPtr<BatchRenderer>, BatchData::BatchDataFunctor>;
private:
    std::unordered_map<ClassId, OwnerPtr<RenderPass>> mRenderPassMap;
    
    BatchMap mBatchMap;

    std::vector<Matrix4> mMatrices;
    SlotsManager mRenderInstancesSlotsManager;
	std::vector<Ptr<MeshRenderer>> mRenderers;
    inline static const u32 mInitialInstances = 2000;
public:
    CRGET(BatchMap)
};
REGISTER_CLASS(RenderPipeline);
