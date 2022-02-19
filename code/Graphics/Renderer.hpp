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
	
	PUB Renderer();
	PUB ~Renderer() override;
	PUB virtual void serialize(JSON& json) const override;
	PUB virtual void deserialize(const JSON& json) override;
	
	PUB void init() override;
	PUB void onComponentAdded() override;

	PRI mutable TransformState mTransformState;

	// Renderer Properties
	PRI mutable Matrix4 mRenderereModelMatrix;
	PRI mutable bool mRenderereModelMatrixGenerated = false;
	PRI mutable bool mVerticesDirty = true;
	PRI mutable std::vector<Vector3> mVertices;
	PRI Vector4 mColor; GET_RC_SET(Color)
	PRI Vector3 mPositionOffset; GET(PositionOffset)
	PRI Rectangle mRegion; GET_RC_SET(Region)
	PRI Rectangle mClipRectangle; GET_RC_SET(ClipRectangle)
	PRI const Mesh* mMesh = nullptr; GET_SET(Mesh)
	PRI Material* mMaterial = nullptr; GET_SET(Material)
	PRI bool mInvertAxisX = false; GET_SET(InvertAxisX)
	PRI bool mIsLineMode = false; GET_SET(IsLineMode)
	PRI i32 mDepth = 0; GET_SET(Depth)
	PRI bool mUseDepth = true; GET_SET(UseDepth) // overrides Z with Depth
	PRI f32 mRenderDistance = 0.0f; GET_SET(RenderDistance)
	
	PRI Chunk* mChunk = nullptr; GET_SET(Chunk)
	PRI Batch* mBatch = nullptr; GET_SET(Batch)

	PUB void setPositionOffset (const Vector3& newPositionOffset);
	PUB bool getIsWorldSpace() const;
	PUB const Matrix4& getRendererModelMatrix(bool force = false) const;
	PUB const std::vector<Vector3>& getVertices(bool force = false) const;
	PUB bool hasClipRectangle() const;

	// Animation
	PRI std::map<std::string, Animation> mAnimations; GET_RC(Animations)
	PRI Animation* mCurrentAnimation = nullptr; GET(CurrentAnimation)

	PUB void setCurrentAnimation(const std::string& name);
	PUB void addAnimation(const std::string& name, const Animation& animation);
    PUB void removeAnimation(const std::string& name);
	PUB bool hasAnimations() const;
	PUB void updateAnimation();

	PUB void onDestroy() override;
};