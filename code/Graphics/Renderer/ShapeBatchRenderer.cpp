#include "Graphics/Renderer/ShapeBatchRenderer.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Material/Shader/ShaderDefault.hpp"
#include "Graphics/Material/Shader/ShaderManager.hpp"

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
    PoolHandler<Material> lineMaterial = GET_SYSTEM(MaterialManager).createMaterial<ShaderDefault>(materialData);
    mShader = GET_SYSTEM(ShaderManager).createShader<ShaderDefault>();
    // mShader->compileShader("Shape", HashedString(std::to_string(lineMaterial->getID())), mGPUVertexBuffersContainer);

    mShader->getGPUProgram()->bindUniformBuffer(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData));
}

void ShapeBatchRenderer::render()
{
//    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::MULTISAMPLE);
//    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::DEPTH_TEST);
//    GET_SYSTEM(GPUInterface).setDepthFunc(GPUDepthFunc::LEQUAL);
//    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::BLEND);
//    GET_SYSTEM(GPUInterface).setBlendFunc(GPUBlendFactor::SRC_ALPHA, GPUBlendFactor::ONE_MINUS_SRC_ALPHA);

    PROFILER_CPU()
	if (mShapesCounter > 0)
	{
		mShader->getGPUProgram()->enable();

		mGPUVertexBuffersContainer.enable();
        mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mPosition).resize(mPositionBuffer.size());
        mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mColor).resize(mColorBuffer.size());
        mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mPosition).setDataArray(mPositionBuffer);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mColor).setDataArray(mColorBuffer);
//		GET_SYSTEM(GPUInterface).drawElements(GPUDrawPrimitive::LINES, mIndicesBuffer.size(), mShapesCounter, false);
        mGPUVertexBuffersContainer.disable();

        mShader->getGPUProgram()->disable();

		mPositionBuffer.clear();
		mColorBuffer.clear();
		mShapesCounter = 0;
	}

//    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::BLEND);
//    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::DEPTH_TEST);
//    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::MULTISAMPLE);
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