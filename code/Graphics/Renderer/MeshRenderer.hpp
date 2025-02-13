#pragma once

#include "Core/ECS/Component.hpp"
#include "SpacePartition/OcTree.hpp"

#include "Scene/Transform.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/Shader/GPUShader.hpp"
#include "Graphics/Renderer/TextureAnimation/TextureAnimation.hpp"

class InstancedMeshRenderer;
class TextureAnimation;

class RendererData
{
public:
    GPUShaderStencilData mGPUShaderStencilData;
    WeakPtr<const GPUMesh> mMesh;
    Matrix4 mMeshInstanceMatrix = Matrix4::smIdentity;
    WeakPtr<GPUShader> mShader;
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
    WeakPtr<GPUShaderPropertiesInstance> mGPUShaderPropertiesInstance;
    bool mUpdateMatrix = false;

public:
    HashedString mCurrentTextureAnimationKey;

public:
    CRGET(RendererModelMatrix)
    CRGET(RendererData)
    RGET(GPUShaderPropertiesInstance)
    CRGET_SET(RenderSlot)
    CRGET_SET(InstanceSlot)
    GET_SET(UpdateMatrix)
};
REGISTER_CLASS(MeshRenderer);
