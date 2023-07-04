#pragma once

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class Chunk;
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

class MeshRenderer: public ComponentWithData<RendererData>
{
    GENERATE_METADATA(MeshRenderer)
	DECLARE_SERIALIZATION()
	
public:
    ClassId getEngineSystemComponentId() const override { return getClassIdStatic(); }
    
    virtual void init(RendererData& data) override;
    void onComponentAdded() override;
    bool getIsWorldSpace() const;
    void update();
    void onDestroy() override;
    bool hasValidChunk() const;
    void setMaterial(Ptr<const Material> material);
    void setColor(const Vector4& color);

protected:
    virtual void preUpdate() {}
    virtual void updatePositions();
    virtual void updateTextureCoords();

private:
    void calculateRendererModelMatrix();

protected:
	OwnerPtr<Mesh> mMeshInstance;
    bool mRegenerateColor = false;
    bool mRegeneratePositions = false;
    bool mRegenerateTextureCoords = false;

private:
    Vector4 mColor;
    Ptr<Batch> mBatch;
    Ptr<const Chunk> mChunk;
    Matrix4 mRendererModelMatrix;

public:
    CGET(MeshInstance)
    CRGET_SET(Batch)
    CRGET_SET(Chunk)
    CRGET(RendererModelMatrix)
};
