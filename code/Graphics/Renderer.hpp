#pragma once

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh.hpp"

class Chunk;
class Batch;

class Renderer: public Component
{
    GENERATE_METADATA(Renderer)

	PRI TransformState mTransformState;

	PRI Matrix4 mRendererModelMatrix; GET_RC_SET(RendererModelMatrix)
	PRI bool mRendererModelMatrixGenerated = false;
	PRI bool mVerticesDirty = true;
	PRI std::vector<Vector3> mVertices; GET_RC_SET(Vertices)
	PRI Vector4 mColor; GET_RC_SET(Color)
	PRI Vector3 mPositionOffset; GET(PositionOffset)
	PRI Rectangle mTextureRegion; GET_RC_SET(TextureRegion)
	PRI Rectangle mClipRectangle; GET_RC_SET(ClipRectangle)
	PRI bool mInvertAxisX = false; GET_SET(InvertAxisX)
	PRI bool mIsLineMode = false; GET_SET(IsLineMode)
	PRI i32 mDepth = 0; GET_SET(Depth)
	PRI bool mUseDepth = true; GET_SET(UseDepth) // overrides Z with Depth
	PRI f32 mRenderDistance = 0.0f; GET_SET(RenderDistance)
	PRI std::map<std::string, Animation> mAnimations; GET_RC(Animations)
	PRI Animation* mCurrentAnimation = nullptr; GET(CurrentAnimation)

	PRI Chunk* mChunk = nullptr; GET_SET(Chunk)
	PRI Batch* mBatch = nullptr; GET_SET(Batch)
	PRI const Mesh* mMesh = nullptr; GET_SET(Mesh)
	PRI Material* mMaterial = nullptr; GET_SET(Material)
	
	PUB void serialize(JSON& json) const override;
	PUB void deserialize(const JSON& json) override;
	
	PUB void init() override;
	PUB void onComponentAdded() override;
	PUB void update();

	PUB void setPositionOffset (const Vector3& newPositionOffset);
	PUB bool getIsWorldSpace() const;
	PUB bool hasClipRectangle() const { return mClipRectangle.getSize().len() > MathUtils::FLOAT_EPSILON; }

	PUB void setCurrentAnimation(const std::string& name);
	PUB void addAnimation(const std::string& name, const Animation& animation);
    PUB void removeAnimation(const std::string& name);
	PUB bool hasAnimations() const { return mAnimations.size() > 0; };
	PUB void updateAnimation();

	PUB void onDestroy() override;
};