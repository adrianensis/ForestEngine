#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"

class RenderPipelineData
{
public:
    std::vector<Ptr<PointLight>> mPointLights;
    Ptr<DirectionalLight> mDirectionalLight;
};

class RenderPipeline: public ObjectBase
{
public:
    void init();
    void terminate();
    void addRenderer(Ptr<MeshRenderer> renderer);
    void removeRenderer(Ptr<MeshRenderer> renderer);
    void render(RenderPipelineData& renderData);

private:
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
        renderPass->init(renderPassData);
    }

    template<class T> T_EXTENDS(T, RenderPass)
    Ptr<T> getRenderPass()
    {
        ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        return Ptr<T>::cast(mRenderPassMap.at(renderPassClassId));
    }
private:
    std::unordered_map<ClassId, OwnerPtr<RenderPass>> mRenderPassMap;
};
REGISTER_CLASS(RenderPipeline);
