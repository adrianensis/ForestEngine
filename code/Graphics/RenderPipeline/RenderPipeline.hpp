#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Light/Light.hpp"

class BatchesManager;
class ShapeBatchRenderer;
class Camera;

class RenderPipelineData
{
public:
    Ptr<Camera> mCamera;
    std::vector<Ptr<Light>> mLights;
};

class RenderPipeline: public ObjectBase
{
public:
    void render(RenderPipelineData& renderData, BatchesManager& batchesManager);
protected:
    void updateGlobalData(RenderPipelineData& renderData, bool isWorldSpace);
    void updateLights(RenderPipelineData& renderData);
};
REGISTER_CLASS(RenderPipeline);
