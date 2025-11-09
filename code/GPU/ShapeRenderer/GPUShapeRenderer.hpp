#pragma once

#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "GPU/Shader/GPUShader.hpp"

class GPUShapeRenderer
{
public:
	GPUShapeRenderer() = default;

    void init(Core::Ptr<GPUContext> gpuContext, Core::u32 verticesPerShape);
    void terminate();
    void render();

	void addLine(const Maths::Line& line, const Maths::Vector4& color);

private:
    void addPosition(const Maths::Vector3& position);
    void addColor(const Maths::Vector4& color);
private: 
	Core::Ptr<GPUContext> mGPUContext;
	Core::WeakPtr<GPUShader> mShader;
    GPUVertexBuffersContainer mGPUVertexBuffersContainer;
	std::vector<Maths::Vector3> mPositionBuffer;
	std::vector<Maths::Vector4> mColorBuffer;
	std::vector<Core::u32> mIndicesBuffer;

protected:
	Core::u32 mMaxShapes = 10000;
	Core::u32 mShapesCounter = 0;
	Core::u32 mVerticesPerShape = 0;
	Core::u32 mMaxVertices = 0;
};
