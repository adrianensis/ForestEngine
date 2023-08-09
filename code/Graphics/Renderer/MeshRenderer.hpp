#pragma once

#include "Scene/Component.hpp"
#include "SpacePartition/OcTreeElement.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class Batch;

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
        mThisObjectId == other.mThisObjectId and 
        mStencilFunction == other.mStencilFunction;
	}
};

class RendererData: public ComponentData
{
public:
    StencilData mStencilData;
    bool mIsInstanced = false;
    Ptr<const Mesh> mMesh;
    Ptr<const Material> mMaterial;
    Vector3 mPositionOffset;
};

class MeshRenderer: public ComponentWithData<RendererData>, public IOcTreeElement
{
    GENERATE_METADATA(MeshRenderer)
	DECLARE_SERIALIZATION()
	
public:
    ClassId getEngineSystemComponentId() const override { return getClassIdStatic(); }
	virtual bool isOcTreeElementStatic() const override;
	virtual bool isOcTreeTransformChanged() const override;

    virtual void init(RendererData& data) override;
    void onComponentAdded() override;
    bool getIsWorldSpace() const;
    void preUpdate();
    void update();
    void onDestroy() override;
    void setMaterial(Ptr<const Material> material);
    void setColor(const Vector4& color);
    void setDepth(i32 depth);
    void setInvertAxisX(bool invertAxisX);
    void setTextureRegion(const Rectangle& textureRegion);

protected:
    void updatePositions();
    void updateTextureCoords();

private:
    void calculateRendererModelMatrix();
    const TextureAnimation* getCurrentTextureAnimation() const;
    void updateTextureRegion();

protected:
	OwnerPtr<Mesh> mMeshInstance;
    bool mRegenerateColor = false;
    bool mRegeneratePositions = false;
    bool mRegenerateTextureCoords = false;

private:
    Vector4 mColor;
    Ptr<Batch> mBatch;
    Matrix4 mRendererModelMatrix;
    mutable bool mRendererModelMatrixDirty = true;

public:
    std::string mCurrentTextureAnimationKey;
    bool mUseDepth = false; // overrides Z with Depth

private:
    TextureAnimationUpdater mCurrentTextureAnimationUpdater;
    Rectangle mTextureRegion = Rectangle(Vector2(0.0, 0.0), Vector2(1.0, 1.0));
    bool mInvertAxisX = false;
    i32 mDepth = 0;

public:
    CGET(MeshInstance)
    CRGET_SET(Batch)
    CRGET(RendererModelMatrix)
};