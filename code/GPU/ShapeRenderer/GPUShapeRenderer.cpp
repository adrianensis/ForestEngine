#include "GPU/ShapeRenderer/GPUShapeRenderer.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"
#include "GPU/Shader/ShaderBuilder/GPUShaderBuilder.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/Shader/BuiltIn/GPUShaderDefault.hpp"

void GPUShapeRenderer::terminate()
{
	mGPUVertexBuffersContainer.terminate();
	mPositionBuffer.clear();
	mColorBuffer.clear();
	mIndicesBuffer.clear();

	mShapesCounter = 0;
}

void GPUShapeRenderer::init(GPUContext* gpuContext, GPU::u32 verticesPerShape)
{
    mGPUContext = gpuContext;
	mVerticesPerShape = verticesPerShape;
    mMaxVertices = mMaxShapes * mVerticesPerShape;

	mPositionBuffer.reserve(mMaxVertices); // 2 vertex per line * 3 floats per vertex
	mColorBuffer.reserve(mMaxVertices); // 2 vertex per line * 4 floats per vertex
	mIndicesBuffer.reserve(mMaxVertices); // 1 index per vertex

    GPUVertexBufferData bufferDataPosition(GPUShaderDefinitions::VertexInput::mPosition);
    mGPUVertexBuffersContainer.addVertexBuffer(mGPUContext, bufferDataPosition, mMaxVertices, false);
    GPUVertexBufferData bufferDataColor(GPUShaderDefinitions::VertexInput::mColor);
    mGPUVertexBuffersContainer.addVertexBuffer(mGPUContext, bufferDataColor, mMaxVertices, false);

    FOR_RANGE(i, 0, mMaxVertices)
    {
        mIndicesBuffer.push_back(i);
    }
    // mGPUVertexBuffersContainer.enable();
    mGPUVertexBuffersContainer.setIndicesBuffer(mGPUContext, GPUShaderDefinitions::PrimitiveTypes::mUnsignedInt, mIndicesBuffer.size(), false);
    mGPUVertexBuffersContainer.getIndicesBuffer().setDataArray(mIndicesBuffer);
    // mGPUVertexBuffersContainer.disable();

    GPUShaderData shaderData;
    // mShader = (GPUShaderManager).createShader<GPUShaderDefault>(shaderData);
    // mShader->compileShader("Shape", std::string(std::to_string(lineShader->getID())), mGPUVertexBuffersContainer);

    // mShader->getGPUShader()->bindUniformBuffer(GPUInstance::getInstance().getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData));
}

void GPUShapeRenderer::render()
{
//    (GPUInterface).enableFlag(GPUFlags::MULTISAMPLE);
//    (GPUInterface).enableFlag(GPUFlags::DEPTH_TEST);
//    (GPUInterface).setDepthFunc(GPUDepthFunc::LEQUAL);
//    (GPUInterface).enableFlag(GPUFlags::BLEND);
//    (GPUInterface).setBlendFunc(GPUBlendFactor::SRC_ALPHA, GPUBlendFactor::ONE_MINUS_SRC_ALPHA);

    PROFILER_CPU()
	if (mShapesCounter > 0)
	{
		// mShader->getGPUShader()->enable();

		mGPUVertexBuffersContainer.enable(mGPUContext);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mPosition).resize(mPositionBuffer.size());
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mColor).resize(mColorBuffer.size());
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mPosition).setDataArray(mPositionBuffer);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mColor).setDataArray(mColorBuffer);
//		(GPUInterface).drawElements(GPUDrawPrimitive::LINES, mIndicesBuffer.size(), mShapesCounter, false);
        mGPUVertexBuffersContainer.disable();

        // mShader->getGPUShader()->disable();

		mPositionBuffer.clear();
		mColorBuffer.clear();
		mShapesCounter = 0;
	}

//    (GPUInterface).disableFlag(GPUFlags::BLEND);
//    (GPUInterface).disableFlag(GPUFlags::DEPTH_TEST);
//    (GPUInterface).disableFlag(GPUFlags::MULTISAMPLE);
}

void GPUShapeRenderer::addPosition(const Maths::Vector3& position)
{
    PROFILER_CPU()
	mPositionBuffer.push_back(position);
}

void GPUShapeRenderer::addColor(const Maths::Vector4& color)
{
    PROFILER_CPU()
	mColorBuffer.push_back(color);
}

void GPUShapeRenderer::addLine(const Maths::Line& line, const Maths::Vector4& color)
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