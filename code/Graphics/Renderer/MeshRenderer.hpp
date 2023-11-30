#pragma once

#include "Scene/Component.hpp"
#include "SpacePartition/OcTree.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class BatchRenderer;

class StencilData
{
public:
    bool mUseStencil = false;
    u32 mMaskStencilValue = 0;
    u32 mStencilFunction = GL_NOTEQUAL;
    u32 mStencilPassOp = GL_KEEP;
    ObjectId mMaskObjectId = 0;
    ObjectId mThisObjectId = 0;

    bool operator==(const StencilData& other) const
	{
        return
        mUseStencil == other.mUseStencil and
        mMaskStencilValue == other.mMaskStencilValue and
        mMaskObjectId == other.mMaskObjectId and
        mThisObjectId == other.mThisObjectId && 
        mStencilFunction == other.mStencilFunction;
	}
};

class RendererData
{
public:
    StencilData mStencilData;
    bool mIsInstanced = false;
    Ptr<const Mesh> mMesh;
    Ptr<const Material> mMaterial;
};

class MeshRenderer: public Component, public IOcTreeElement
{
    GENERATE_METADATA(MeshRenderer)
	DECLARE_SERIALIZATION()
	
public:
    ClassId getSystemComponentId() const override { return getClassDefinitionStatic().mId; }

    void init(const RendererData& data);
    void onComponentAdded() override;
    bool getIsWorldSpace() const;
    void update();
    void onDestroy() override;
    void setMaterial(Ptr<const Material> material);
    void setColor(const Vector4& color);
    void setDepth(i32 depth);
    void setInvertAxisX(bool invertAxisX);
    void setTextureRegion(const Rectangle& textureRegion);
    void setPositionOffset(const Vector3& positionOffset);

protected:
    void updatePositions();
    void updateTextureCoords();

private:
    void calculateRendererModelMatrix();
    const TextureAnimation* getCurrentTextureAnimation() const;
    void updateTextureRegion();

private:
    RendererData mRendererData;
	OwnerPtr<Mesh> mMeshInstance;
    bool mRegenerateColor = false;
    bool mRegeneratePositions = false;
    bool mRegenerateTextureCoords = false;
    Vector4 mColor;
    Ptr<BatchRenderer> mBatchRenderer;
    Matrix4 mRendererModelMatrix;
    bool mRendererPositionOffsetDirty = true;
    TextureAnimationUpdater mCurrentTextureAnimationUpdater;
    Rectangle mTextureRegion = Rectangle(Vector2(0.0, 0.0), Vector2(1.0, 1.0));
    bool mInvertAxisX = false;
    i32 mDepth = 0;
    Vector3 mPositionOffset;

public:
    std::string mCurrentTextureAnimationKey;
    bool mUseDepth = false; // overrides Z with Depth

public:
    CGET(MeshInstance)
    CRGET_SET(BatchRenderer)
    CRGET(RendererModelMatrix)
    CRGET(RendererData)
};
