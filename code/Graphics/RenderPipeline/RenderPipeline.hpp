#pragma once

#include "Core/Module.hpp"
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
    GENERATE_METADATA(RenderPipeline)

public:
    void render(RenderPipelineData& renderData, BatchesManager& batchesManager, ShapeBatchRenderer& shapeBatchRenderer, ShapeBatchRenderer& shapeBatchRendererScreenSpace);
protected:
    void updateGlobalData(RenderPipelineData& renderData, bool isWorldSpace);
    void updateLights(RenderPipelineData& renderData);
};
