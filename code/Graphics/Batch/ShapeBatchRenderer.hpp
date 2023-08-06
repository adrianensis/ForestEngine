#pragma once

#include "Core/Module.hpp"
#include "Graphics/Shaders/Shader.hpp"


class ShapeBatchRenderer: public ObjectBase
{
	GENERATE_METADATA(ShapeBatchRenderer)

public:
	ShapeBatchRenderer() { }

    ~ShapeBatchRenderer() override;

    void init(bool isWorldSpace, u32 verticesPerShape);
    void terminate();
    void render();

	void addLine(const Line& shape, const Vector4& color);

private:
    void bind();
    void addPosition(const Vector3& position);
    void addColor(const Vector4& color);
private: 
	OwnerPtr<Shader> mShaderLine;
	u32 mVAO = 0;
	u32 mVBOPosition = 0;
	u32 mVBOColor = 0;
	u32 mEBO = 0;
	std::vector<f32> mPositionBuffer;
	std::vector<f32> mColorBuffer;
	std::vector<u32> mIndicesBuffer;

	f32 mSize = 0.0f;

protected:
	u32 mMaxShapes = 10000;
	u32 mShapesCounter = 0;
	u32 mPositionsPerShape = 0;
	bool mIsWorldSpace = true;
};
