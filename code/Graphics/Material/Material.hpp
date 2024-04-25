#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Material/MaterialRuntime/MaterialRuntime.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"

class GPUProgram;
class Mesh;

DECLARE_ENUM(TextureMap,
    /** Dummy value.
    *
    *  No texture, but the value to be used as 'texture semantic'
    *  (#aiMaterialProperty::mSemantic) for all material properties
    *  *not* related to textures.
    */
    NONE = 0, "NONE",

    /** LEGACY API MATERIALS
     * Legacy refers to materials which
     * Were originally implemented in the specifications around 2000.
     * These must never be removed, as most engines support them.
     */

    /** The texture is combined with the result of the diffuse
     *  lighting equation.
     *  OR
     *  PBR Specular/Glossiness
     */
    DIFFUSE, "DIFFUSE",

    /** The texture is combined with the result of the specular
     *  lighting equation.
     *  OR
     *  PBR Specular/Glossiness
     */
    SPECULAR_GLOSSINESS, "SPECULAR_GLOSSINESS",

    /** The texture is combined with the result of the ambient
     *  lighting equation.
     */
    AMBIENT, "AMBIENT",

    /** The texture is added to the result of the lighting
     *  calculation. It isn't influenced by incoming light.
     */
    EMISSIVE, "EMISSIVE",

    /** The texture is a height map.
     *
     *  By convention, higher gray-scale values stand for
     *  higher elevations from the base height.
     */
    HEIGHT, "HEIGHT",

    /** The texture defines the glossiness of the material.
     *
     *  The glossiness is in fact the exponent of the specular
     *  (phong) lighting equation. Usually there is a conversion
     *  function defined to map the linear color values in the
     *  texture to a suitable exponent. Have fun.
    */
    GLOSSINESS, "GLOSSINESS",

    /** The texture defines per-pixel opacity.
     *
     *  Usually 'white' means opaque and 'black' means
     *  'transparency'. Or quite the opposite. Have fun.
    */
    OPACITY, "OPACITY",

    /** Displacement texture
     *
     *  The exact purpose and format is application-dependent.
     *  Higher color values stand for higher vertex displacements.
    */
    DISPLACEMENT, "DISPLACEMENT",

    /** Lightmap texture (aka Ambient Occlusion)
     *
     *  Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
     *  covered by this material property. The texture contains a
     *  scaling value for the final color value of a pixel. Its
     *  intensity is not affected by incoming light.
    */
    LIGHTMAP, "LIGHTMAP",

    /** Reflection texture
     *
     * Contains the color of a perfect mirror reflection.
     * Rarely used, almost never for real-time applications.
    */
    REFLECTION, "REFLECTION",

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

/** Unknown texture
 *
 *  A texture reference that does not match any of the definitions
 *  above is considered to be 'unknown'. It is still imported,
 *  but is excluded from any further post-processing.
*/
// UNKNOWN

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

    template<class T> T_EXTENDS(T, MaterialRuntime)
    void init(const MaterialData& materialData, u32 id)
    {
        mMaterialRuntime = OwnerPtr<MaterialRuntime>::moveCast(OwnerPtr<T>::newObject());
        internalInit(materialData, id);
    }

    void terminate();
    virtual void onPoolFree() override { terminate(); };
    void enable() const;
    void disable() const;
    bool hasTexture(TextureMap textureMap) const;

    void bindToShader(Ptr<GPUProgram> shader) const;

private:
    void internalInit(const MaterialData& materialData, u32 id);
protected:
    virtual void loadTextures();

protected:
    MaterialData mMaterialData;

    std::array<PoolHandler<Texture>, (u32)TextureMap::MAX> mTextures;

    u32 mID = 0;

    OwnerPtr<MaterialRuntime> mMaterialRuntime;

public:
    CRGET(MaterialData)
    GET(ID)
    CGET(MaterialRuntime)
};
REGISTER_CLASS(Material);
