#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"

class GPUProgram;
class Mesh;

class MaterialData
{
public:
	bool mReceiveLight = true;
	bool mReceiveShadows = true;
	bool mCastShadows = true;
    GPUCullFaceType mCullFaceType = GPUCullFaceType::BACK;
    bool mAllowInstances = true;
    u32 mMaxInstances = 100;
    bool mIsFont = false;
    FontData mFontData;
    std::unordered_map<HashedString, TextureBinding> mTextureBindings;
    std::unordered_map<HashedString, TextureAnimation> mTextureAnimations;

    GenericObjectBuffer mSharedMaterialPropertiesBlockBuffer;
    ClassDefinition mSharedMaterialPropertiesBlockClass;

    template<class T>
    void setSharedMaterialPropertiesBlock()
    {
        mSharedMaterialPropertiesBlockBuffer.set<T>();
        mSharedMaterialPropertiesBlockClass = ClassManager::getClassMetadata<T>().mClassDefinition;
    }

    u32 getSharedMaterialPropertiesBlockBufferSize() const
    {
        return mSharedMaterialPropertiesBlockBuffer.getByteBuffer().size();
    }
    bool allowInstances() const
    {
        return mAllowInstances && mMaxInstances > 0 && getSharedMaterialPropertiesBlockBufferSize() > 0;
    }

    u32 getMaxInstances() const
    {
        return mAllowInstances ? mMaxInstances : 1;
    }
};

class Material;

class MaterialInstance
{
public:
    PoolHandler<Material> mMaterial;
    GenericObjectBuffer mMaterialPropertiesBlockBuffer;
    bool mDirty = false;
};

class Material: public ObjectBase
{
public:
    Material() = default;

    template<class T> T_EXTENDS(T, Shader)
    void init(const MaterialData& materialData, u32 id)
    {
        mShader = OwnerPtr<Shader>::moveCast(OwnerPtr<T>::newObject());
        internalInit(materialData, id);
    }

    void terminate();
    virtual void onPoolFree() override { terminate(); };
    void enable() const;
    void disable() const;

private:
    void internalInit(const MaterialData& materialData, u32 id);

protected:
    MaterialData mMaterialData;
    u32 mID = 0;
    OwnerPtr<Shader> mShader;

public:
    CRGET(MaterialData)
    GET(ID)
    GET(Shader)
};
REGISTER_CLASS(Material);
