#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"

class GPUProgram;
class Mesh;

DECLARE_ENUM(TextureMap,
    /** PBR Materials
     * PBR definitions from maya and other modelling packages now use this standard.
     * This was originally introduced around 2012.
     * Support for this is in game engines like Godot, Unreal or Unity3D.
     * Modelling packages which use this are very common now.
     */

    BASE_COLOR, "BASE_COLOR",
    NORMAL, "NORMAL",
    METALLIC_ROUGHNESS, "METALIC_ROUGHNESS",
    AMBIENT_OCCLUSION, "AMBIENT_OCCLUSION",

    /** PBR Material Modifiers
    * Some modern renderers have further PBR modifiers that may be overlaid
    * on top of the 'base' PBR materials for additional realism.
    * These use multiple texture maps, so only the base type is directly defined
    */

    /** Sheen
    * Generally used to simulate textiles that are covered in a layer of microfibers
    * eg velvet
    * https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_sheen
    */
    SHEEN, "SHEEN",

    /** Clearcoat
    * Simulates a layer of 'polish' or 'laquer' layered on top of a PBR substrate
    * https://autodesk.github.io/standard-surface/#closures/coating
    * https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_clearcoat
    */
    CLEARCOAT, "CLEARCOAT",

    /** Transmission
    * Simulates transmission through the surface
    * May include further information such as wall thickness
    */
    TRANSMISSION, "TRANSMISSION"
);

class MaterialTextureBinding
{
public:
    std::string mPath;
    GPUPipelineStage mStage = GPUPipelineStage::NONE;
};

class MaterialData
{
public:
	bool mReceiveLight = true;
	bool mCastShadows = true;
    bool mIsSkinned = false;
    GPUCullFaceType mCullFaceType = GPUCullFaceType::BACK;
    bool mAllowInstances = true;
    u32 mMaxInstances = 100;
    bool mIsFont = false;
    FontData mFontData;
    std::array<MaterialTextureBinding, (u32)TextureMap::MAX> mTextureBindings;
    std::unordered_map<std::string, TextureAnimation> mTextureAnimations;

    GenericObjectBuffer mSharedMaterialPropertiesBlockBuffer;

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
    bool hasTexture(TextureMap textureMap) const;

    void bindToShader(Ptr<GPUProgram> gpuProgram) const;

private:
    void internalInit(const MaterialData& materialData, u32 id);
protected:
    virtual void loadTextures();

protected:
    MaterialData mMaterialData;

    std::array<PoolHandler<Texture>, (u32)TextureMap::MAX> mTextures;

    u32 mID = 0;

    OwnerPtr<Shader> mShader;

public:
    CRGET(MaterialData)
    GET(ID)
    CGET(Shader)
};
REGISTER_CLASS(Material);
