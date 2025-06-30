#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "GPU/ShapeRenderer/GPUShapeRenderer.hpp"

class DebugRenderer : public Core::System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    void drawLine(const Line& line, f32 thickness = 1, GeometricSpace geometricSpace = GeometricSpace::WORLD, Vector4 color = Vector4(1,1,1,1));
    void drawRectangle(const Rectangle& rectangle, f32 thickness= 1, GeometricSpace geometricSpace = GeometricSpace::WORLD, Vector4 color = Vector4(1,1,1,1));
    void drawCube(const Cube& cube, f32 thickness= 1, GeometricSpace geometricSpace = GeometricSpace::WORLD, Vector4 color = Vector4(1,1,1,1));

public:
	GPUShapeRenderer mShapeBatchRenderer;
	GPUShapeRenderer mShapeBatchRendererScreenSpace;
};
REGISTER_CLASS(DebugRenderer);
