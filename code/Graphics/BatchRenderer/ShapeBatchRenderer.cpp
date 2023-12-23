#include "Graphics/BatchRenderer/ShapeBatchRenderer.hpp"
#include "Graphics/GPU/GPUShader.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Engine/EngineConfig.hpp"
#include "Graphics/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/MaterialManager.hpp"

ShapeBatchRenderer::~ShapeBatchRenderer() 
{

}
void ShapeBatchRenderer::terminate()
{
	mGPUVertexBuffersLayout.terminate();
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

    mGPUVertexBuffersLayout.init(false);
    mGPUVertexBuffersLayout.setIndicesBuffer(GPUBuiltIn::PrimitiveTypes::mUnsignedInt);
    GPUVertexBufferData bufferDataPosition(GPUBuiltIn::VertexInput::mPosition);
    mVBOPosition = mGPUVertexBuffersLayout.createBuffer(bufferDataPosition);
    GPUVertexBufferData bufferDataColor(GPUBuiltIn::VertexInput::mColor);
    mVBOColor = mGPUVertexBuffersLayout.createBuffer(bufferDataColor);

    FOR_RANGE(i, 0, mMaxVertices)
    {
        mIndicesBuffer.push_back(i);
    }

    mGPUVertexBuffersLayout.getIndicesBuffer().resize(mIndicesBuffer.size());
    mGPUVertexBuffersLayout.getIndicesBuffer().setDataArray(mIndicesBuffer);

	mGPUVertexBuffersLayout.disable();

    MaterialData materialData;
    materialData.mReceiveLight = false;
    materialData.mUseVertexColor = true;
    materialData.mUseNormals = false;
    materialData.mUseModelMatrix = false;
    Ptr<const Material> lineMaterial = GET_SYSTEM(MaterialManager).createMaterial(materialData);

	mShaderLine = OwnerPtr<GPUShader>::newObject();

    ShaderBuilder sbVert;
    sbVert.createVertexShader(mGPUVertexBuffersLayout, lineMaterial);
    ShaderBuilder sbFrag;
    sbFrag.createFragmentShader(mGPUVertexBuffersLayout, lineMaterial);

    mShaderLine = OwnerPtr<GPUShader>::newObject();
    std::string stringShderVert = sbVert.getCode();
    LOG(stringShderVert);
    std::string stringShderFrag = sbFrag.getCode();
    LOG(stringShderFrag);
    mShaderLine->initFromFileContents(stringShderVert, stringShderFrag);

    mShaderLine->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).mGlobalMatricesBuffer);
}

void ShapeBatchRenderer::render()
{
    PROFILER_CPU()
	if (mShapesCounter > 0)
	{
		mShaderLine->enable();

		mGPUVertexBuffersLayout.enable();
        mGPUVertexBuffersLayout.getBuffer(mVBOPosition).resize(mPositionBuffer.size());
        mGPUVertexBuffersLayout.getBuffer(mVBOColor).resize(mColorBuffer.size());
        mGPUVertexBuffersLayout.getBuffer(mVBOPosition).setDataArray(mPositionBuffer);
        mGPUVertexBuffersLayout.getBuffer(mVBOColor).setDataArray(mColorBuffer);
		GET_SYSTEM(GPUInterface).drawElements(GL_LINES, mIndicesBuffer.size(), mShapesCounter, false);
        mGPUVertexBuffersLayout.disable();

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