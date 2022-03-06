#pragma once

// CPP_IGNORE

#include "Core/Module.hpp"
#include "Graphics/Material/Shader.hpp"

class ShapeBatchRenderer: public ObjectBase
{
	GENERATE_METADATA(ShapeBatchRenderer)

protected:
	u32 mMaxShapes = 0;
	u32 mShapesCounter = 0;
	u32 mVerticesPerShape = 0;

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

private:
	void bind();

	void addPosition(const Vector3& position);
	void addColor(const Vector4& color);

	template<class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	void addSpecificShape(const T& shape, const Vector4& color)
	{

	}

public:
	ShapeBatchRenderer() { }
	~ShapeBatchRenderer() override;

	void init(bool isWorldSpace, u32 verticesPerShape);
	void render();
	template<class T, typename = std::enable_if_t<std::is_base_of<Shape, T>::value> >
	void add(const T& shape, const Vector4& color)
	{
		if(mShapesCounter < mMaxShapes)
		{
			addSpecificShape<T>(shape, color);
			mShapesCounter++;
		}
	}
};

// Shape Batch Renderer Specialization
template<>
void ShapeBatchRenderer::addSpecificShape<Line>(const Line& shape, const Vector4& color);

// To handle different batches for different shapes
class ShapeBatchRendererMap: public ObjectBase
{
    GENERATE_METADATA(ShapeBatchRendererMap)
	
private:
	std::map<ClassId, ShapeBatchRenderer*> mShapeBatchMap;
	bool mIsWorldSpace = true;

public:
	void render();
	void terminate();

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

	GET_SET(IsWorldSpace)
};
