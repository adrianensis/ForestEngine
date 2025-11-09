#include "Graphics/Debug/DebugRenderer.hpp"
#include "GPU/GPUInstance.hpp"

void DebugRenderer::init()
{
	mShapeBatchRenderer.init(GPUInstance::getInstance().mGPUContext, 2);
	mShapeBatchRendererScreenSpace.init(GPUInstance::getInstance().mGPUContext, 2);
}

void DebugRenderer::terminate()
{    
	mShapeBatchRenderer.terminate();
	mShapeBatchRendererScreenSpace.terminate();
}

void DebugRenderer::drawLine(const Maths::Line& line, Core::f32 thickness /*= 1*/, Maths::GeometricSpace geometricSpace /*= true*/, Maths::Vector4 color /*= Maths::Vector4(1,1,1,1)*/)
{
    PROFILER_CPU()
	if (geometricSpace == Maths::GeometricSpace::WORLD)
	{
		mShapeBatchRenderer.addLine(line, color);
	}
	else
	{
		mShapeBatchRendererScreenSpace.addLine(line, color);
	}
}

void DebugRenderer::drawRectangle(const Maths::Rectangle& rectangle, Core::f32 thickness/*= 1*/, Maths::GeometricSpace geometricSpace /*= true*/, Maths::Vector4 color /*= Maths::Vector4(1,1,1,1)*/)
{
    PROFILER_CPU()
	const Maths::Vector3& leftTopFront = rectangle.getLeftTopFront();
	const Maths::Vector3& size = rectangle.getSize();
	drawLine(Maths::Line(Maths::Vector3(leftTopFront.x, leftTopFront.y, leftTopFront.z), Maths::Vector3(leftTopFront.x, leftTopFront.y - size.y, leftTopFront.z)), thickness, geometricSpace, color);
	drawLine(Maths::Line(Maths::Vector3(leftTopFront.x, leftTopFront.y - size.y, leftTopFront.z), Maths::Vector3(leftTopFront.x + size.x, leftTopFront.y - size.y, leftTopFront.z)), thickness, geometricSpace, color);
	drawLine(Maths::Line(Maths::Vector3(leftTopFront.x + size.x, leftTopFront.y - size.y, leftTopFront.z), Maths::Vector3(leftTopFront.x + size.x, leftTopFront.y, leftTopFront.z)), thickness, geometricSpace, color);
	drawLine(Maths::Line(Maths::Vector3(leftTopFront.x + size.x, leftTopFront.y, leftTopFront.z), Maths::Vector3(leftTopFront.x, leftTopFront.y, leftTopFront.z)), thickness, geometricSpace, color);
}

void DebugRenderer::drawCube(const Maths::Cube& cube, Core::f32 thickness/*= 1*/, Maths::GeometricSpace geometricSpace /*= true*/, Maths::Vector4 color /*= Maths::Vector4(1,1,1,1)*/)
{
    PROFILER_CPU()
	const Maths::Vector3& leftTopFront = cube.getLeftTopFront();
	const Maths::Vector3& size = cube.getSize();
	Maths::Vector3 leftTopBack = leftTopFront - Maths::Vector3(0,0,size.z);
	drawRectangle(Maths::Rectangle(leftTopFront, size), thickness, geometricSpace, color);
	drawRectangle(Maths::Rectangle(leftTopBack, size), thickness, geometricSpace, color);
	drawLine(Maths::Line(leftTopFront + Maths::Vector3::smZero, leftTopBack + Maths::Vector3::smZero), thickness, geometricSpace, color);
	drawLine(Maths::Line(leftTopFront + Maths::Vector3(size.x,0,0), leftTopBack + Maths::Vector3(size.x,0,0)), thickness, geometricSpace, color);
	drawLine(Maths::Line(leftTopFront + Maths::Vector3(size.x,-size.y,0), leftTopBack + Maths::Vector3(size.x,-size.y,0)), thickness, geometricSpace, color);
	drawLine(Maths::Line(leftTopFront + Maths::Vector3(0,-size.y,0), leftTopBack + Maths::Vector3(0,-size.y,0)), thickness, geometricSpace, color);
}