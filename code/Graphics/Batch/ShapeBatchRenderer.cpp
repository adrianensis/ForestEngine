#include "Graphics/Batch/ShapeBatchRenderer.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Core/EngineConfig.hpp"


ShapeBatchRenderer::~ShapeBatchRenderer() 
{
	Memory::deleteObject(mShaderLine);

	RenderContext::deleteVAO(mVAO);
	RenderContext::deleteVBO(mVBOPosition);
	RenderContext::deleteVBO(mEBO);

	mPositionBuffer.clear();
	mColorBuffer.clear();
	mIndicesBuffer.clear();

	mShapesCounter = 0;
}

void ShapeBatchRenderer::init(bool isWorldSpace, u32 verticesPerShape)
{
	mMaxShapes = EngineConfig::getInstance().getConfig().at("line").at("count").get<f32>();

	mIsWorldSpace = isWorldSpace;

	mPositionsPerShape = verticesPerShape;

	mPositionBuffer.reserve(mMaxShapes * 2 * 3); // 2 vertex per line * 3 floats per vertex
	mColorBuffer.reserve(mMaxShapes * 2 * 4); // 2 vertex per line * 4 floats per vertex
	mIndicesBuffer.reserve(mMaxShapes * 2);		 // 1 index per vertex

	mShaderLine = Memory::newObject<Shader>();
	mShaderLine->initDebug();

	bind();
}

void ShapeBatchRenderer::render()
{
	if (mShapesCounter > 0)
	{
		mShaderLine->enable();

		RenderContext::enableVAO(mVAO);

		RenderContext::enableProperty(0);
		RenderContext::enableProperty(1);

		const Matrix4& projectionMatrix = RenderEngine::getInstance().mCamera.get().mProjectionMatrix;
		const Matrix4& viewMatrix = RenderEngine::getInstance().mCamera.get().mViewMatrix;

		mShaderLine->addMatrix(mIsWorldSpace ? projectionMatrix : Matrix4::getIdentity(), "projectionMatrix");
		mShaderLine->addMatrix(mIsWorldSpace ? viewMatrix : Matrix4::getIdentity(), "viewMatrix");

		RenderContext::setDataVBO(mVBOPosition, mPositionBuffer);
		RenderContext::setDataVBO(mVBOColor, mColorBuffer);
		RenderContext::drawLines(mShapesCounter);

		RenderContext::disableProperty(0);
		RenderContext::disableProperty(1);

		RenderContext::enableVAO(0);

		mPositionBuffer.clear();
		mColorBuffer.clear();
		mShapesCounter = 0;
	}
}

void ShapeBatchRenderer::bind()
{
	mVAO = RenderContext::createVAO();
	mVBOPosition = RenderContext::createVBO();
	RenderContext::attribute(0, 3, GL_FLOAT, 3 * sizeof(f32), 0, 0);
	mVBOColor = RenderContext::createVBO();
	RenderContext::attribute(1, 4, GL_FLOAT, 4 * sizeof(f32), 0, 0);
	mEBO = RenderContext::createEBO();

	RenderContext::resizeVBO(mVBOPosition, mPositionBuffer.capacity());
	RenderContext::resizeVBO(mVBOColor, mColorBuffer.capacity());

	FOR_RANGE(i, 0, mMaxShapes * mPositionsPerShape)
	{
		mIndicesBuffer.push_back(i);
	}

	RenderContext::resizeEBO(mEBO, mIndicesBuffer.size());
	RenderContext::setDataEBORaw(mEBO, mIndicesBuffer);

	RenderContext::enableVAO(0);
}

void ShapeBatchRenderer::addPosition(const Vector3& position)
{
	mPositionBuffer.push_back(position.x);
	mPositionBuffer.push_back(position.y);
	mPositionBuffer.push_back(position.z);
}

void ShapeBatchRenderer::addColor(const Vector4& color)
{
	mColorBuffer.push_back(color.x);
	mColorBuffer.push_back(color.y);
	mColorBuffer.push_back(color.z);
	mColorBuffer.push_back(color.w);
}
template<>
void ShapeBatchRenderer::addSpecificShape<Line>(const Line& shape, const Vector4& color)
{
addPosition(shape.getStart());
addPosition(shape.getEnd());

addColor(color);
addColor(color);
}


void ShapeBatchRendererMap::render()
{
	FOR_MAP(it, mShapeBatchMap)
	{
		it->second->render();
	}
}

void ShapeBatchRendererMap::terminate()
{
	MAP_DELETE_CONTENT(mShapeBatchMap);
}
