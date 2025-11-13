#pragma once

#include "Engine/Core.hpp"
#include "GPU/ShapeRenderer/GPUShapeRenderer.hpp"

class DebugRenderer : public System::System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    void drawLine(const Maths::Line& line, Core::f32 thickness = 1, Maths::GeometricSpace geometricSpace = Maths::GeometricSpace::WORLD, Maths::Vector4 color = Maths::Vector4(1,1,1,1));
    void drawRectangle(const Maths::Rectangle& rectangle, Core::f32 thickness= 1, Maths::GeometricSpace geometricSpace = Maths::GeometricSpace::WORLD, Maths::Vector4 color = Maths::Vector4(1,1,1,1));
    void drawCube(const Maths::Cube& cube, Core::f32 thickness= 1, Maths::GeometricSpace geometricSpace = Maths::GeometricSpace::WORLD, Maths::Vector4 color = Maths::Vector4(1,1,1,1));

public:
	GPUShapeRenderer mShapeBatchRenderer;
	GPUShapeRenderer mShapeBatchRendererScreenSpace;
};
REGISTER_CLASS(DebugRenderer, System);
