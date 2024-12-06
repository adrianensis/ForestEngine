#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/GPU/Buffer/GPUUniformBuffer.hpp"
#include "Graphics/GPU/Buffer/GPUBuffersContainer.hpp"
#include "Graphics/GPU/Core/GPUBuiltIn.hpp"

class MaterialData
{
public:
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

// REF: Merge Material and Shader classes? rename as ShaderMaterial? or just Shader?
class Material: public ObjectBase, public IPoolable
{
public:
    Material() = default;

    void init(const MaterialData& materialData, u32 id);

    void terminate();
    virtual void onPoolFree() override { terminate(); };

protected:
    MaterialData mMaterialData;
    u32 mID = 0;

public:
    CRGET(MaterialData)
    GET(ID)
};
REGISTER_CLASS(Material);
