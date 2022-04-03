#pragma once

#include "Core/Module.hpp"
#include "Graphics/Material/Shader.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/ShapeBatchRenderer.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Core/EngineConfig.hpp"
#endif

class ShapeBatchRenderer: public ObjectBase
{
	GENERATE_METADATA(ShapeBatchRenderer)

public:
	ShapeBatchRenderer() { }

	CPP ~ShapeBatchRenderer() override
	{
		DELETE(mShaderLine);

		glDeleteVertexArrays(1, &mVAO);
		glDeleteBuffers(1, &mVBOPosition);
		glDeleteBuffers(1, &mEBO);

		mPositionBuffer.clear();
		mColorBuffer.clear();
		mIndicesBuffer.clear();

		mShapesCounter = 0;
	}

	CPP void init(bool isWorldSpace, u32 verticesPerShape)
	{
		mMaxShapes = EngineConfig::getInstance().getConfig().at("line").at("count").get<f32>();

		mIsWorldSpace = isWorldSpace;

		mVerticesPerShape = verticesPerShape;

		mPositionBuffer.reserve(mMaxShapes * 2 * 3); // 2 vertex per line * 3 floats per vertex
		mColorBuffer.reserve(mMaxShapes * 2 * 4); // 2 vertex per line * 4 floats per vertex
		mIndicesBuffer.reserve(mMaxShapes * 2);		 // 1 index per vertex

		mShaderLine = NEW(Shader);
		mShaderLine->initDebug();

		bind();
	}

	CPP void render()
	{
		if (mShapesCounter > 0)
		{
			mShaderLine->enable();

			RenderContext::enableVAO(mVAO);

			RenderContext::enableProperty(0);
			RenderContext::enableProperty(1);

			const Matrix4& projectionMatrix = RenderEngine::getInstance().getCamera().get().getProjectionMatrix();
			const Matrix4& viewMatrix = RenderEngine::getInstance().getCamera().get().getViewMatrix();

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

	template<class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	void add(const T& shape, const Vector4& color)
	{
		if(mShapesCounter < mMaxShapes)
		{
			addSpecificShape<T>(shape, color);
			mShapesCounter++;
		}
	}

private:
	CPP void bind()
	{
		mVAO = RenderContext::createVAO();
		mVBOPosition = RenderContext::createVBO(3, 0);
		mVBOColor = RenderContext::createVBO(4, 1);
		mEBO = RenderContext::createEBO();

		RenderContext::resizeVBO(mVBOPosition, mPositionBuffer.capacity());
		RenderContext::resizeVBO(mVBOColor, mColorBuffer.capacity());

		FOR_RANGE(i, 0, mMaxShapes * mVerticesPerShape)
		{
			mIndicesBuffer.push_back(i);
		}

		RenderContext::resizeEBO(mEBO, mIndicesBuffer.size());
		RenderContext::setDataEBO(mEBO, mIndicesBuffer);

		RenderContext::enableVAO(0);
	}

	CPP void addPosition(const Vector3& position)
	{
		mPositionBuffer.push_back(position.x);
		mPositionBuffer.push_back(position.y);
		mPositionBuffer.push_back(position.z);
	}

	CPP void addColor(const Vector4& color)
	{
		mColorBuffer.push_back(color.x);
		mColorBuffer.push_back(color.y);
		mColorBuffer.push_back(color.z);
		mColorBuffer.push_back(color.w);
	}

	template<class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	void addSpecificShape(const T& shape, const Vector4& color)
	{

	}

private: 
	Shader* mShaderLine = nullptr;
	u32 mVAO = 0;
	u32 mVBOPosition = 0;
	u32 mVBOColor = 0;
	u32 mEBO = 0;
	std::vector<f32> mPositionBuffer;
	std::vector<f32> mColorBuffer;
	std::vector<u16> mIndicesBuffer;

	f32 mSize = 0.0f;

	bool mIsWorldSpace = true;
protected:
	u32 mMaxShapes = 0;
	u32 mShapesCounter = 0;
	u32 mVerticesPerShape = 0;
};

// Shape Batch Renderer Specialization
template<>
void ShapeBatchRenderer::addSpecificShape<Line>(const Line& shape, const Vector4& color);

#ifdef CPP_INCLUDE
template<>
void ShapeBatchRenderer::addSpecificShape<Line>(const Line& shape, const Vector4& color)
{
	addPosition(shape.getStart());
	addPosition(shape.getEnd());

	addColor(color);
	addColor(color);
}
#endif

// To handle different batches for different shapes
class ShapeBatchRendererMap: public ObjectBase
{
    GENERATE_METADATA(ShapeBatchRendererMap)
	
public:
	CPP void render()
	{
		FOR_MAP(it, mShapeBatchMap)
		{
			it->second->render();
		}
	}

	CPP void terminate()
	{
		MAP_DELETE_CONTENT(mShapeBatchMap);
	}

	template<class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	void add(const T& shape, bool isWorldSpace = true, const Vector4& color = Vector4(1, 1, 1, 1))
	{
		ClassId shapeClassId = T::getClassIdStatic();
		if(!MAP_CONTAINS(mShapeBatchMap, shapeClassId))
		{
			ShapeBatchRenderer* shapeBatchRenderer = NEW(ShapeBatchRenderer);
			shapeBatchRenderer->init(mIsWorldSpace, shape.getVerticesCount());
			MAP_INSERT(mShapeBatchMap, shapeClassId, shapeBatchRenderer)
		}

		mShapeBatchMap.at(shapeClassId)->add<T>(shape, color);
	}

private:
	std::map<ClassId, ShapeBatchRenderer*> mShapeBatchMap;
	bool mIsWorldSpace = true;

public:
	GET_SET(IsWorldSpace)
};