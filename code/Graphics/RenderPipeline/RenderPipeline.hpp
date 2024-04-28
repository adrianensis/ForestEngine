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
    void onRendererAdded(Ptr<MeshRenderer> renderer);
    void onRendererRemoved(Ptr<MeshRenderer> renderer);
    void render(RenderPipelineData& renderData);
protected:
    void updateGlobalData(RenderPipelineData& renderData, bool isWorldSpace);
    void updateLights(RenderPipelineData& renderData);

private:
    std::unordered_map<ClassId, OwnerPtr<RenderPass>> mRenderPassMap;
};
REGISTER_CLASS(RenderPipeline);
