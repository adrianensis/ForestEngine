#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"

class ShapeBatchRenderer
{
public:
	ShapeBatchRenderer() = default;

    void init(bool isWorldSpace, u32 verticesPerShape);
    void terminate();
    void render();

	void addLine(const Line& line, const Vector4& color);

private:
    void addPosition(const Vector3& position);
    void addColor(const Vector4& color);
private: 
	OwnerPtr<GPUProgram> mShaderLine;
    GPUVertexBuffersContainer mGPUVertexBuffersContainer;
	std::vector<Vector3> mPositionBuffer;
	std::vector<Vector4> mColorBuffer;
	std::vector<u32> mIndicesBuffer;

protected:
	u32 mMaxShapes = 10000;
	u32 mShapesCounter = 0;
	u32 mVerticesPerShape = 0;
	u32 mMaxVertices = 0;
	bool mIsWorldSpace = true;
};
