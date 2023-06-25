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
    u32 mStencilValue = 0x00;
    u32 mStencilFunction = GL_NOTEQUAL;
    ObjectId mMaskObjectId = 0;
    ObjectId mThisObjectId = 0;

    bool operator==(const StencilData& other) const
	{
        return
        mUseStencil == other.mUseStencil and
        mStencilValue == other.mStencilValue and
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
    void changeMaterial(Ptr<const Material> material);

    Matrix4 getRendererModelMatrix() const;

protected:
    virtual void preUpdate() {}
    virtual void updatePositions();
    virtual void updateTextureCoords();

public:
    Vector4 mColor;

protected:
	OwnerPtr<Mesh> mMeshInstance;

private:
    Ptr<const Batch> mBatch;
    Ptr<const Chunk> mChunk;

public:
    CGET(MeshInstance)
    CRGET_SET(Batch)
    CRGET_SET(Chunk)
};
