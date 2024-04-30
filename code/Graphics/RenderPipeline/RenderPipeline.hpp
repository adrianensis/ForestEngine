#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"

class ShapeBatchRenderer;
class Camera;

class RenderPipelineData
{
public:
    Ptr<Camera> mCamera;
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
    void updateGlobalData(RenderPipelineData& renderData, bool isWorldSpace);
    void updateLights(RenderPipelineData& renderData);

    template<class T> T_EXTENDS(T, RenderPass)
    void initRenderPass(const RenderPassData& renderPassData)
    {
        ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        mRenderPassMap.insert_or_assign(
            renderPassClassId,
            OwnerPtr<RenderPass>::moveCast(OwnerPtr<T>::newObject())
        );
        mRenderPassMap.at(renderPassClassId)->init(renderPassData);
    }
    template<class T> T_EXTENDS(T, RenderPass)
    void renderRenderPass()
    {
        ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        mRenderPassMap.at(renderPassClassId)->render();
    }
private:
    std::unordered_map<ClassId, OwnerPtr<RenderPass>> mRenderPassMap;
};
REGISTER_CLASS(RenderPipeline);
