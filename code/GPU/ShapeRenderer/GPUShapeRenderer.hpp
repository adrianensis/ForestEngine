#pragma once

#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "GPU/Shader/GPUShader.hpp"

class GPUShapeRenderer
{
public:
	GPUShapeRenderer() = default;

    void init(GPUContext* gpuContext, GPU::u32 verticesPerShape);
    void terminate();
    void render();

	void addLine(const Maths::Line& line, const Maths::Vector4& color);

private:
    void addPosition(const Maths::Vector3& position);
    void addColor(const Maths::Vector4& color);
private: 
	GPUContext* mGPUContext = nullptr;
	GPUShader* mShader = nullptr;
    GPUVertexBuffersContainer mGPUVertexBuffersContainer;
	std::vector<Maths::Vector3> mPositionBuffer;
	std::vector<Maths::Vector4> mColorBuffer;
	std::vector<GPU::u32> mIndicesBuffer;

protected:
	GPU::u32 mMaxShapes = 10000;
	GPU::u32 mShapesCounter = 0;
	GPU::u32 mVerticesPerShape = 0;
	GPU::u32 mMaxVertices = 0;
};
