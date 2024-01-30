#include "Graphics/BatchRenderer/ShapeBatchRenderer.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Engine/EngineConfig.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/Material/MaterialManager.hpp"

ShapeBatchRenderer::~ShapeBatchRenderer() 
{

}
void ShapeBatchRenderer::terminate()
{
	mGPUBuffersLayout.terminate();
	mPositionBuffer.clear();
	mColorBuffer.clear();
	mIndicesBuffer.clear();

	mShapesCounter = 0;
}

void ShapeBatchRenderer::init(bool isWorldSpace, u32 verticesPerShape)
{
	mIsWorldSpace = isWorldSpace;

	mVerticesPerShape = verticesPerShape;
    mMaxVertices = mMaxShapes * mVerticesPerShape;

	mPositionBuffer.reserve(mMaxVertices); // 2 vertex per line * 3 floats per vertex
	mColorBuffer.reserve(mMaxVertices); // 2 vertex per line * 4 floats per vertex
	mIndicesBuffer.reserve(mMaxVertices); // 1 index per vertex

    mGPUBuffersLayout.init(false);
    mGPUBuffersLayout.setIndicesBuffer(GPUBuiltIn::PrimitiveTypes::mUnsignedInt);
    GPUVertexBufferData bufferDataPosition(GPUBuiltIn::VertexInput::mPosition);
    mGPUBuffersLayout.createVertexBuffer(bufferDataPosition);
    GPUVertexBufferData bufferDataColor(GPUBuiltIn::VertexInput::mColor);
    mGPUBuffersLayout.createVertexBuffer(bufferDataColor);

    FOR_RANGE(i, 0, mMaxVertices)
    {
        mIndicesBuffer.push_back(i);
    }

    mGPUBuffersLayout.getIndicesBuffer().resize(mIndicesBuffer.size());
    mGPUBuffersLayout.getIndicesBuffer().setDataArray(mIndicesBuffer);

	mGPUBuffersLayout.disable();

    MaterialData materialData;
    materialData.mReceiveLight = false;
    materialData.mUseNormals = false;
    materialData.mUseModelMatrix = false;
    materialData.mUseVertexColor = true;
    Ptr<const Material> lineMaterial = GET_SYSTEM(MaterialManager).createMaterial(materialData);

    mShaderLine = ShaderUtils::createShader(mGPUBuffersLayout, lineMaterial);

    mShaderLine->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).mGlobalDataBuffer);
}

void ShapeBatchRenderer::render()
{
    PROFILER_CPU()
	if (mShapesCounter > 0)
	{
		mShaderLine->enable();

		mGPUBuffersLayout.enable();
        mGPUBuffersLayout.getVertexBuffer(GPUBuiltIn::VertexInput::mPosition).resize(mPositionBuffer.size());
        mGPUBuffersLayout.getVertexBuffer(GPUBuiltIn::VertexInput::mColor).resize(mColorBuffer.size());
        mGPUBuffersLayout.getVertexBuffer(GPUBuiltIn::VertexInput::mPosition).setDataArray(mPositionBuffer);
        mGPUBuffersLayout.getVertexBuffer(GPUBuiltIn::VertexInput::mColor).setDataArray(mColorBuffer);
		GET_SYSTEM(GPUInterface).drawElements(GPUDrawPrimitive::LINES, mIndicesBuffer.size(), mShapesCounter, false);
        mGPUBuffersLayout.disable();

		mPositionBuffer.clear();
		mColorBuffer.clear();
		mShapesCounter = 0;
	}
}

void ShapeBatchRenderer::addPosition(const Vector3& position)
{
    PROFILER_CPU()
	mPositionBuffer.push_back(position);
}

void ShapeBatchRenderer::addColor(const Vector4& color)
{
    PROFILER_CPU()
	mColorBuffer.push_back(color);
}

void ShapeBatchRenderer::addLine(const Line& line, const Vector4& color)
{
    PROFILER_CPU()
    if(mShapesCounter < mMaxShapes)
    {
        addPosition(line.getStart());
        addPosition(line.getEnd());

        addColor(color);
        addColor(color);
        mShapesCounter++;
    }
}