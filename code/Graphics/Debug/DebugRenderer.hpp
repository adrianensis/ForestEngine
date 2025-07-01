#pragma once

#include "Core/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "GPU/ShapeRenderer/GPUShapeRenderer.hpp"

class DebugRenderer : public System::System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    void drawLine(const Line& line, Core::f32 thickness = 1, GeometricSpace geometricSpace = GeometricSpace::WORLD, Vector4 color = Vector4(1,1,1,1));
    void drawRectangle(const Rectangle& rectangle, Core::f32 thickness= 1, GeometricSpace geometricSpace = GeometricSpace::WORLD, Vector4 color = Vector4(1,1,1,1));
    void drawCube(const Cube& cube, Core::f32 thickness= 1, GeometricSpace geometricSpace = GeometricSpace::WORLD, Vector4 color = Vector4(1,1,1,1));

public:
	GPUShapeRenderer mShapeBatchRenderer;
	GPUShapeRenderer mShapeBatchRendererScreenSpace;
};
REGISTER_CLASS(DebugRenderer);
