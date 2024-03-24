#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/Systems/System.hpp"
#include "Graphics/BatchRenderer/ShapeBatchRenderer.hpp"

class DebugRenderer : public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    void drawLine(const Line& line, f32 thickness = 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));
    void drawRectangle(const Rectangle& rectangle, f32 thickness= 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));
    void drawCube(const Cube& cube, f32 thickness= 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));

public:
	ShapeBatchRenderer mShapeBatchRenderer;
	ShapeBatchRenderer mShapeBatchRendererScreenSpace;
};
REGISTER_CLASS(DebugRenderer);
