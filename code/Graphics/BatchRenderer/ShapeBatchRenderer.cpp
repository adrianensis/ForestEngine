#include "Graphics/BatchRenderer/ShapeBatchRenderer.hpp"
#include "Graphics/GPU/GPUShader.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Engine/EngineConfig.hpp"


ShapeBatchRenderer::~ShapeBatchRenderer() 
{

}
void ShapeBatchRenderer::terminate()
{
	GET_SYSTEM(GPUInterface).deleteVertexBufferLayout(mVertexBufferLayout);
	GET_SYSTEM(GPUInterface).deleteBuffer(mVBOPosition);
	GET_SYSTEM(GPUInterface).deleteBuffer(mEBO);

	mPositionBuffer.clear();
	mColorBuffer.clear();
	mIndicesBuffer.clear();

	mShapesCounter = 0;
}

void ShapeBatchRenderer::init(bool isWorldSpace, u32 verticesPerShape)
{
	mIsWorldSpace = isWorldSpace;

	mPositionsPerShape = verticesPerShape;

	mPositionBuffer.reserve(mMaxShapes * 2 * 3); // 2 vertex per line * 3 floats per vertex
	mColorBuffer.reserve(mMaxShapes * 2 * 4); // 2 vertex per line * 4 floats per vertex
	mIndicesBuffer.reserve(mMaxShapes * 2);		 // 1 index per vertex

	mShaderLine = OwnerPtr<GPUShader>::newObject();
	mShaderLine->initDebug();

	bind();
}

void ShapeBatchRenderer::render()
{
    PROFILER_CPU()
	if (mShapesCounter > 0)
	{
		mShaderLine->enable();

		GET_SYSTEM(GPUInterface).enableVertexBufferLayout(mVertexBufferLayout);

		GET_SYSTEM(GPUInterface).enableAttribute(0);
		GET_SYSTEM(GPUInterface).enableAttribute(1);

		const Matrix4& projectionMatrix = GET_SYSTEM(RenderEngine).mCamera->mProjectionMatrix;
		const Matrix4& viewMatrix = GET_SYSTEM(RenderEngine).mCamera->mViewMatrix;

		mShaderLine->addMatrix(mIsWorldSpace ? projectionMatrix : Matrix4::smIdentity, "projectionMatrix");
		mShaderLine->addMatrix(mIsWorldSpace ? viewMatrix : Matrix4::smIdentity, "viewMatrix");

		GET_SYSTEM(GPUInterface).setBufferDataArray(GPUBufferType::VERTEX, mVBOPosition, mPositionBuffer);
		GET_SYSTEM(GPUInterface).setBufferDataArray(GPUBufferType::VERTEX, mVBOColor, mColorBuffer);
		GET_SYSTEM(GPUInterface).drawLines(mShapesCounter);

		GET_SYSTEM(GPUInterface).disableAttribute(0);
		GET_SYSTEM(GPUInterface).disableAttribute(1);

		GET_SYSTEM(GPUInterface).enableVertexBufferLayout(0);

		mPositionBuffer.clear();
		mColorBuffer.clear();
		mShapesCounter = 0;
	}
}

void ShapeBatchRenderer::bind()
{
	mVertexBufferLayout = GET_SYSTEM(GPUInterface).createVertexBufferLayout();
	mVBOPosition = GET_SYSTEM(GPUInterface).createBuffer();
	GET_SYSTEM(GPUInterface).attribute(0, 3, GL_FLOAT, 3 * sizeof(f32), 0, 0);
	mVBOColor = GET_SYSTEM(GPUInterface).createBuffer();
	GET_SYSTEM(GPUInterface).attribute(1, 4, GL_FLOAT, 4 * sizeof(f32), 0, 0);
	mEBO = GET_SYSTEM(GPUInterface).createBuffer();

	GET_SYSTEM(GPUInterface).resizeBuffer(GPUBufferType::VERTEX, mVBOPosition, sizeof(f32), mPositionBuffer.capacity());
	GET_SYSTEM(GPUInterface).resizeBuffer(GPUBufferType::VERTEX, mVBOColor, sizeof(f32), mColorBuffer.capacity());

	FOR_RANGE(i, 0, mMaxShapes * mPositionsPerShape)
	{
		mIndicesBuffer.push_back(i);
	}

	GET_SYSTEM(GPUInterface).resizeBuffer(GPUBufferType::INDEX, mEBO, sizeof(u32), mIndicesBuffer.size());
	GET_SYSTEM(GPUInterface).setBufferDataArray(GPUBufferType::INDEX, mEBO, mIndicesBuffer);

	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(0);
}

void ShapeBatchRenderer::addPosition(const Vector3& position)
{
    PROFILER_CPU()
	mPositionBuffer.push_back(position.x);
	mPositionBuffer.push_back(position.y);
	mPositionBuffer.push_back(position.z);
}

void ShapeBatchRenderer::addColor(const Vector4& color)
{
    PROFILER_CPU()
	mColorBuffer.push_back(color.x);
	mColorBuffer.push_back(color.y);
	mColorBuffer.push_back(color.z);
	mColorBuffer.push_back(color.w);
}

void ShapeBatchRenderer::addLine(const Line& shape, const Vector4& color)
{
    PROFILER_CPU()
    if(mShapesCounter < mMaxShapes)
    {
        addPosition(shape.getStart());
        addPosition(shape.getEnd());

        addColor(color);
        addColor(color);
        mShapesCounter++;
    }
}