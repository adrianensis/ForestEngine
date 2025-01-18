#pragma once

#include "Core/ECS/Component.hpp"
#include "SpacePartition/OcTree.hpp"

#include "Scene/Transform.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "Graphics/Shader/Shader.hpp"

class InstancedMeshRenderer;
class ShaderInstance;

class RendererData
{
public:
    ShaderStencilData mShaderStencilData;
    WeakPtr<const GPUMesh> mMesh;
    Matrix4 mMeshInstanceMatrix = Matrix4::smIdentity;
    WeakPtr<Shader> mShader;
    std::unordered_set<ClassId> mRenderPassIDs;
};

class MeshRenderer: public Component, public IOcTreeElement
{
    
	DECLARE_SERIALIZATION()
	
public:
    ClassId getComponentTypeId() const override;

    void init(const RendererData& data);
    void onComponentAdded() override;
    void onDestroy() override;
    void update();

private:
    void calculateRendererModelMatrix();
    const TextureAnimation* getCurrentTextureAnimation() const;
    void updateTextureRegion();

private:
    RendererData mRendererData;
    Slot mInstanceSlot;
    Slot mRenderSlot;
    Matrix4 mRendererModelMatrix;
    TextureAnimationUpdater mCurrentTextureAnimationUpdater;
    WeakPtr<ShaderInstance> mShaderInstance;
    bool mUpdateMatrix = false;

public:
    HashedString mCurrentTextureAnimationKey;

public:
    CRGET(RendererModelMatrix)
    CRGET(RendererData)
    RGET(ShaderInstance)
    CRGET_SET(RenderSlot)
    CRGET_SET(InstanceSlot)
    GET_SET(UpdateMatrix)
};
REGISTER_CLASS(MeshRenderer);
