#include "Graphics/Renderer/BatchRenderer/ShapeBatchRenderer.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/RenderSharedContext.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Material/Shader/ShaderDefault.hpp"

void ShapeBatchRenderer::terminate()
{
	mGPUVertexBuffersContainer.terminate();
	mPositionBuffer.clear();
	mColorBuffer.clear();
	mIndicesBuffer.clear();

	mShapesCounter = 0;
}

void ShapeBatchRenderer::init(u32 verticesPerShape)
{
	mVerticesPerShape = verticesPerShape;
    mMaxVertices = mMaxShapes * mVerticesPerShape;

	mPositionBuffer.reserve(mMaxVertices); // 2 vertex per line * 3 floats per vertex
	mColorBuffer.reserve(mMaxVertices); // 2 vertex per line * 4 floats per vertex
	mIndicesBuffer.reserve(mMaxVertices); // 1 index per vertex

    GPUVertexBufferData bufferDataPosition(GPUBuiltIn::VertexInput::mPosition);
    mGPUVertexBuffersContainer.addVertexBuffer(bufferDataPosition, false);
    GPUVertexBufferData bufferDataColor(GPUBuiltIn::VertexInput::mColor);
    mGPUVertexBuffersContainer.addVertexBuffer(bufferDataColor, false);

	mGPUVertexBuffersContainer.create();

    FOR_RANGE(i, 0, mMaxVertices)
    {
        mIndicesBuffer.push_back(i);
    }
    mGPUVertexBuffersContainer.enable();
    mGPUVertexBuffersContainer.setIndicesBuffer(GPUBuiltIn::PrimitiveTypes::mUnsignedInt, false);
    mGPUVertexBuffersContainer.getIndicesBuffer().resize(mIndicesBuffer.size());
    mGPUVertexBuffersContainer.getIndicesBuffer().setDataArray(mIndicesBuffer);
    mGPUVertexBuffersContainer.disable();

    MaterialData materialData;
    materialData.mReceiveLight = false;
    PoolHandler<Material> lineMaterial = GET_SYSTEM(MaterialManager).createMaterial<ShaderDefault>(materialData);

    mShaderLine = ShaderUtils::createShader("Shape", mGPUVertexBuffersContainer, {}, lineMaterial.get());

    mShaderLine->bindSharedBuffer(GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData));
}

void ShapeBatchRenderer::render()
{
    PROFILER_CPU()
	if (mShapesCounter > 0)
	{
		mShaderLine->enable();

		mGPUVertexBuffersContainer.enable();
        mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mPosition).resize(mPositionBuffer.size());
        mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mColor).resize(mColorBuffer.size());
        mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mPosition).setDataArray(mPositionBuffer);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mColor).setDataArray(mColorBuffer);
		GET_SYSTEM(GPUInterface).drawElements(GPUDrawPrimitive::LINES, mIndicesBuffer.size(), mShapesCounter, false);
        mGPUVertexBuffersContainer.disable();

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