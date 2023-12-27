#pragma once

#include "Core/Module.hpp"

enum class TextureType
{
    /** Dummy value.
    *
    *  No texture, but the value to be used as 'texture semantic'
    *  (#aiMaterialProperty::mSemantic) for all material properties
    *  *not* related to textures.
    */
    NONE = 0,

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
    DIFFUSE,

    /** The texture is combined with the result of the specular
     *  lighting equation.
     *  OR
     *  PBR Specular/Glossiness
     */
    SPECULAR,

    /** The texture is combined with the result of the ambient
     *  lighting equation.
     */
    AMBIENT,

    /** The texture is added to the result of the lighting
     *  calculation. It isn't influenced by incoming light.
     */
    EMISSIVE,

    /** The texture is a height map.
     *
     *  By convention, higher gray-scale values stand for
     *  higher elevations from the base height.
     */
    HEIGHT,

    /** The texture is a (tangent space) normal-map.
     *
     *  Again, there are several conventions for tangent-space
     *  normal maps. Assimp does (intentionally) not
     *  distinguish here.
     */
    NORMALS,

    /** The texture defines the glossiness of the material.
     *
     *  The glossiness is in fact the exponent of the specular
     *  (phong) lighting equation. Usually there is a conversion
     *  function defined to map the linear color values in the
     *  texture to a suitable exponent. Have fun.
    */
    SHININESS,

    /** The texture defines per-pixel opacity.
     *
     *  Usually 'white' means opaque and 'black' means
     *  'transparency'. Or quite the opposite. Have fun.
    */
    OPACITY,

    /** Displacement texture
     *
     *  The exact purpose and format is application-dependent.
     *  Higher color values stand for higher vertex displacements.
    */
    DISPLACEMENT,

    /** Lightmap texture (aka Ambient Occlusion)
     *
     *  Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
     *  covered by this material property. The texture contains a
     *  scaling value for the final color value of a pixel. Its
     *  intensity is not affected by incoming light.
    */
    LIGHTMAP,

    /** Reflection texture
     *
     * Contains the color of a perfect mirror reflection.
     * Rarely used, almost never for real-time applications.
    */
    REFLECTION,

    /** PBR Materials
     * PBR definitions from maya and other modelling packages now use this standard.
     * This was originally introduced around 2012.
     * Support for this is in game engines like Godot, Unreal or Unity3D.
     * Modelling packages which use this are very common now.
     */

    BASE_COLOR,
    NORMAL_CAMERA,
    EMISSION_COLOR,
    METALNESS,
    DIFFUSE_ROUGHNESS,
    AMBIENT_OCCLUSION,

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
    SHEEN,

    /** Clearcoat
    * Simulates a layer of 'polish' or 'laquer' layered on top of a PBR substrate
    * https://autodesk.github.io/standard-surface/#closures/coating
    * https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_clearcoat
    */
    CLEARCOAT,

    /** Transmission
    * Simulates transmission through the surface
    * May include further information such as wall thickness
    */
    TRANSMISSION,

    // Max enum value
    MAX,

    /** Unknown texture
     *
     *  A texture reference that does not match any of the definitions
     *  above is considered to be 'unknown'. It is still imported,
     *  but is excluded from any further post-processing.
    */
    UNKNOWN,
};

class TextureData
{
public:
    std::string mPath;
    bool mCreateMipMap = true;
    FontData mFontData;
};

class Texture: public ObjectBase
{
    GENERATE_METADATA(Texture)
public:
    ~Texture() override;
    virtual void init(const TextureData& textureData) {};
    void bind() const;

protected:
	u32 mTextureId = 0;
	u32 mWidth = 0;
	u32 mHeight = 0;
	TextureData mTextureData;
};

class TextureImage: public Texture
{
    GENERATE_METADATA(TextureImage)
public:
    virtual void init(const TextureData& textureData) override;
};

class TextureFont: public Texture
{
    GENERATE_METADATA(TextureFont)
public:
    virtual void init(const TextureData& textureData) override;
};
