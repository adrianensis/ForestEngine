#include "Graphics/Batch/ShapeBatchRenderer.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Engine/EngineConfig.hpp"


ShapeBatchRenderer::~ShapeBatchRenderer() 
{

}
void ShapeBatchRenderer::terminate()
{
	GET_SYSTEM(RenderContext).deleteVAO(mVAO);
	GET_SYSTEM(RenderContext).deleteVBO(mVBOPosition);
	GET_SYSTEM(RenderContext).deleteVBO(mEBO);

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

	mShaderLine = OwnerPtr<Shader>::newObject();
	mShaderLine->initDebug();

	bind();
}

void ShapeBatchRenderer::render()
{
    PROFILER_CPU()
	if (mShapesCounter > 0)
	{
		mShaderLine->enable();

		GET_SYSTEM(RenderContext).enableVAO(mVAO);

		GET_SYSTEM(RenderContext).enableProperty(0);
		GET_SYSTEM(RenderContext).enableProperty(1);

		const Matrix4& projectionMatrix = GET_SYSTEM(RenderEngine).mCamera->mProjectionMatrix;
		const Matrix4& viewMatrix = GET_SYSTEM(RenderEngine).mCamera->mViewMatrix;

		mShaderLine->addMatrix(mIsWorldSpace ? projectionMatrix : Matrix4::smIdentity, "projectionMatrix");
		mShaderLine->addMatrix(mIsWorldSpace ? viewMatrix : Matrix4::smIdentity, "viewMatrix");

		GET_SYSTEM(RenderContext).setDataVBO(mVBOPosition, mPositionBuffer);
		GET_SYSTEM(RenderContext).setDataVBO(mVBOColor, mColorBuffer);
		GET_SYSTEM(RenderContext).drawLines(mShapesCounter);

		GET_SYSTEM(RenderContext).disableProperty(0);
		GET_SYSTEM(RenderContext).disableProperty(1);

		GET_SYSTEM(RenderContext).enableVAO(0);

		mPositionBuffer.clear();
		mColorBuffer.clear();
		mShapesCounter = 0;
	}
}

void ShapeBatchRenderer::bind()
{
	mVAO = GET_SYSTEM(RenderContext).createVAO();
	mVBOPosition = GET_SYSTEM(RenderContext).createVBO();
	GET_SYSTEM(RenderContext).attribute(0, 3, GL_FLOAT, 3 * sizeof(f32), 0, 0);
	mVBOColor = GET_SYSTEM(RenderContext).createVBO();
	GET_SYSTEM(RenderContext).attribute(1, 4, GL_FLOAT, 4 * sizeof(f32), 0, 0);
	mEBO = GET_SYSTEM(RenderContext).createEBO();

	GET_SYSTEM(RenderContext).resizeVBO(mVBOPosition, mPositionBuffer.capacity());
	GET_SYSTEM(RenderContext).resizeVBO(mVBOColor, mColorBuffer.capacity());

	FOR_RANGE(i, 0, mMaxShapes * mPositionsPerShape)
	{
		mIndicesBuffer.push_back(i);
	}

	GET_SYSTEM(RenderContext).resizeEBO(mEBO, mIndicesBuffer.size());
	GET_SYSTEM(RenderContext).setDataEBORaw(mEBO, mIndicesBuffer);

	GET_SYSTEM(RenderContext).enableVAO(0);
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