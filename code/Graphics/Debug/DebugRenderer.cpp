#include "Graphics/Debug/DebugRenderer.hpp"

void DebugRenderer::init()
{
	mShapeBatchRenderer.init(2);
	mShapeBatchRendererScreenSpace.init(2);
}

void DebugRenderer::terminate()
{    
	mShapeBatchRenderer.terminate();
	mShapeBatchRendererScreenSpace.terminate();
}

void DebugRenderer::drawLine(const Line& line, f32 thickness /*= 1*/, GeometricSpace geometricSpace /*= true*/, Vector4 color /*= Vector4(1,1,1,1)*/)
{
    PROFILER_CPU()
	if (geometricSpace == GeometricSpace::WORLD)
	{
		mShapeBatchRenderer.addLine(line, color);
	}
	else
	{
		mShapeBatchRendererScreenSpace.addLine(line, color);
	}
}

void DebugRenderer::drawRectangle(const Rectangle& rectangle, f32 thickness/*= 1*/, GeometricSpace geometricSpace /*= true*/, Vector4 color /*= Vector4(1,1,1,1)*/)
{
    PROFILER_CPU()
	const Vector3& leftTopFront = rectangle.getLeftTopFront();
	const Vector3& size = rectangle.getSize();
	drawLine(Line(Vector3(leftTopFront.x, leftTopFront.y, leftTopFront.z), Vector3(leftTopFront.x, leftTopFront.y - size.y, leftTopFront.z)), thickness, geometricSpace, color);
	drawLine(Line(Vector3(leftTopFront.x, leftTopFront.y - size.y, leftTopFront.z), Vector3(leftTopFront.x + size.x, leftTopFront.y - size.y, leftTopFront.z)), thickness, geometricSpace, color);
	drawLine(Line(Vector3(leftTopFront.x + size.x, leftTopFront.y - size.y, leftTopFront.z), Vector3(leftTopFront.x + size.x, leftTopFront.y, leftTopFront.z)), thickness, geometricSpace, color);
	drawLine(Line(Vector3(leftTopFront.x + size.x, leftTopFront.y, leftTopFront.z), Vector3(leftTopFront.x, leftTopFront.y, leftTopFront.z)), thickness, geometricSpace, color);
}

void DebugRenderer::drawCube(const Cube& cube, f32 thickness/*= 1*/, GeometricSpace geometricSpace /*= true*/, Vector4 color /*= Vector4(1,1,1,1)*/)
{
    PROFILER_CPU()
	const Vector3& leftTopFront = cube.getLeftTopFront();
	const Vector3& size = cube.getSize();
	Vector3 leftTopBack = leftTopFront - Vector3(0,0,size.z);
	drawRectangle(Rectangle(leftTopFront, size), thickness, geometricSpace, color);
	drawRectangle(Rectangle(leftTopBack, size), thickness, geometricSpace, color);
	drawLine(Line(leftTopFront + Vector3::smZero, leftTopBack + Vector3::smZero), thickness, geometricSpace, color);
	drawLine(Line(leftTopFront + Vector3(size.x,0,0), leftTopBack + Vector3(size.x,0,0)), thickness, geometricSpace, color);
	drawLine(Line(leftTopFront + Vector3(size.x,-size.y,0), leftTopBack + Vector3(size.x,-size.y,0)), thickness, geometricSpace, color);
	drawLine(Line(leftTopFront + Vector3(0,-size.y,0), leftTopBack + Vector3(0,-size.y,0)), thickness, geometricSpace, color);
}