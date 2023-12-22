#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUShader.hpp"
#include "Graphics/GPU/GPUVertexBuffersLayout.hpp"

class ShapeBatchRenderer: public ObjectBase
{
	GENERATE_METADATA(ShapeBatchRenderer)

public:
	ShapeBatchRenderer() = default;
    ~ShapeBatchRenderer() override;

    void init(bool isWorldSpace, u32 verticesPerShape);
    void terminate();
    void render();

	void addLine(const Line& line, const Vector4& color);

private:
    void addPosition(const Vector3& position);
    void addColor(const Vector4& color);
private: 
	OwnerPtr<GPUShader> mShaderLine;
	u32 mVBOPosition = 0;
	u32 mVBOColor = 0;
    GPUVertexBuffersLayout mGPUVertexBuffersLayout;
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
