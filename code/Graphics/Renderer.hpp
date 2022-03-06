#pragma once

// CPP_IGNORE

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh.hpp"

// CPP_INCLUDE
// #include "generated-code/final/code/Graphics/Renderer.hpp"
// #include "Graphics/Animation/AnimationFrame.hpp"
// #include "Graphics/Material/Shader.hpp"
// #include "Graphics/RenderEngine.hpp"
// #include "Graphics/Camera/Camera.hpp"
// #include "Graphics/Material/Texture.hpp"
// #include "Graphics/Material/MaterialManager.hpp"
// #include "Graphics/MeshPrimitives.hpp"
// #include "Graphics/Animation/Animation.hpp"
// #include "Graphics/Optimizations/Chunk.hpp"
// #include "Graphics/Optimizations/Batch.hpp"
// #include "Scene/Module.hpp"
// CPP_INCLUDE

class Chunk;
class Batch;

class Renderer: public Component
{
    GENERATE_METADATA(Renderer)
	
private:
	TransformState mTransformState;
	Matrix4 mRendererModelMatrix;
	bool mRendererModelMatrixGenerated = false;
	bool mVerticesDirty = true;
	std::vector<Vector3> mVertices;
	Vector4 mColor;
	Vector3 mPositionOffset;
	Rectangle mTextureRegion;
	Rectangle mClipRectangle;
	bool mInvertAxisX = false;
	bool mIsLineMode = false;
	i32 mDepth = 0;
	bool mUseDepth = true; // overrides Z with Depth
	f32 mRenderDistance = 0.0f;
	std::map<std::string, Animation> mAnimations;
	std::string mCurrentAnimationName;
	Chunk* mChunk = nullptr;
	Batch* mBatch = nullptr;
	const Mesh* mMesh = nullptr;
	Material* mMaterial = nullptr;

private:
	void updateAnimation();

public:
	void init() override;
	void onComponentAdded() override;
	void onDestroy() override;
	void update();
	void setPositionOffset (const Vector3& newPositionOffset);
	bool getIsWorldSpace() const;
	bool hasClipRectangle() const { return mClipRectangle.getSize().len() > MathUtils::FLOAT_EPSILON; }
	const Animation* getCurrentAnimation() const { return &mAnimations.at(mCurrentAnimationName); }

	void serialize(JSON& json) const override;
	void deserialize(const JSON& json) override;

	// CPP
	// template<class T>
	// void test(int a = 0, Material* mat = nullptr) const
	// {
	// 	if(1 == 1)
	// 	{
	// 		float a = 0;

	// 	}

	// 	;
	// 	;
	// 	std::vector<std::map<std::string, Material*>> vec;


	// 	update();
	// 	this->update();
	// }

	// CPP void test_ASDASDSD(int a = 0, Material* mat = nullptr) const
	// {
	// 	if(1 == 1)
	// 	{
	// 		float a = 0;

	// 	}

	// 	;
	// 	;
	// 	std::vector<std::map<std::string, Material*>> vec;
	// }

	CRGET_SET(RendererModelMatrix)
	CRGET_SET(Vertices)
	CRGET_SET(Color)
	GET(PositionOffset)
	CRGET_SET(TextureRegion)
	CRGET_SET(ClipRectangle)
	GET_SET(InvertAxisX)
	GET_SET(IsLineMode)
	GET_SET(Depth)
	GET_SET(UseDepth)
	GET_SET(RenderDistance)
	RGET(Animations)
	GET_SET(CurrentAnimationName)
	GET_SET(Chunk)
	GET_SET(Batch)
	GET_SET(Mesh)
	GET_SET(Material)
};