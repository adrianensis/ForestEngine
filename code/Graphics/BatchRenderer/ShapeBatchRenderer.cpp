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
	GET_SYSTEM(GPUInterface).deleteVertexBufferLayout(mVertexBufferLayout);
	GET_SYSTEM(GPUInterface).deleteBuffer(mEBO);

	mPositionBuffer.clear();
	mColorBuffer.clear();
	mIndicesBuffer.clear();

	mShapesCounter = 0;
}

void ShapeBatchRenderer::init(bool isWorldSpace, u32 verticesPerShape)
{
	mIsWorldSpace = isWorldSpace;

	mVerticesPerShape = verticesPerShape;

	mPositionBuffer.reserve(mMaxShapes * mVerticesPerShape); // 2 vertex per line * 3 floats per vertex
	mColorBuffer.reserve(mMaxShapes * mVerticesPerShape); // 2 vertex per line * 4 floats per vertex
	mIndicesBuffer.reserve(mMaxShapes * mVerticesPerShape);		 // 1 index per vertex

    MaterialData materialData;
    materialData.mReceiveLight = false;
    materialData.mUseVertexColor = true;
    materialData.mUseNormals = false;
    materialData.mUseModelMatrix = false;
    Ptr<const Material> lineMaterial = GET_SYSTEM(MaterialManager).createMaterial(materialData);

    mVertexBufferLayout = GET_SYSTEM(GPUInterface).createVertexBufferLayout();
	mEBO = GET_SYSTEM(GPUInterface).createBuffer();

    mGPUVertexBuffersLayout.init(false);
    GPUVertexBufferData bufferDataPosition(GPUBuiltIn::VertexInput::mPosition);
    mVBOPosition = mGPUVertexBuffersLayout.addBuffer(bufferDataPosition);
    GPUVertexBufferData bufferDataColor(GPUBuiltIn::VertexInput::mColor);
    mVBOColor = mGPUVertexBuffersLayout.addBuffer(bufferDataColor);

	FOR_RANGE(i, 0, mMaxShapes * mVerticesPerShape)
	{
		mIndicesBuffer.push_back(i);
	}

	GET_SYSTEM(GPUInterface).resizeBuffer(GPUBufferType::INDEX, mEBO, sizeof(u32), mIndicesBuffer.size());
	GET_SYSTEM(GPUInterface).setBufferDataArray(GPUBufferType::INDEX, mEBO, mIndicesBuffer);

	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(0);

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

		GET_SYSTEM(GPUInterface).enableVertexBufferLayout(mVertexBufferLayout);
        mGPUVertexBuffersLayout.getBuffer(mVBOPosition).resize(mPositionBuffer.size());
        mGPUVertexBuffersLayout.getBuffer(mVBOColor).resize(mColorBuffer.size());
        mGPUVertexBuffersLayout.getBuffer(mVBOPosition).setDataArray(mPositionBuffer);
        mGPUVertexBuffersLayout.getBuffer(mVBOColor).setDataArray(mColorBuffer);
		GET_SYSTEM(GPUInterface).drawElements(GL_LINES, mIndicesBuffer.size(), mShapesCounter, false);
		GET_SYSTEM(GPUInterface).enableVertexBufferLayout(0);

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