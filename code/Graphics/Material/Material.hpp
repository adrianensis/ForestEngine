#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Shader/ShaderManager.hpp"

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

class Material: public ObjectBase, public IPoolable
{
public:
    Material() = default;

    template<class T> T_EXTENDS(T, Shader)
    void init(const MaterialData& materialData, u32 id)
    {
        LOG_TRACE()
        PROFILER_CPU()
        mShader = GET_SYSTEM(ShaderManager).createShader<T>();
        internalInit(materialData, id);
    }

    void terminate();
    virtual void onPoolFree() override { terminate(); };

private:
    void internalInit(const MaterialData& materialData, u32 id);

protected:
    MaterialData mMaterialData;
    u32 mID = 0;
    Ptr<Shader> mShader;

public:
    CRGET(MaterialData)
    GET(ID)
    GET(Shader)
};
REGISTER_CLASS(Material);
