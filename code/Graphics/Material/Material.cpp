#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUShader.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Model.hpp"

void Material::init(const MaterialData& materialData, u32 id)
{
    mMaterialData = materialData;
	mID = id;

    addInstancedProperty(MaterialInstancedPropertiesGPUData::mColor);
    addInstancedProperty(MaterialInstancedPropertiesGPUData::mTextureRegionLeftTop);
    addInstancedProperty(MaterialInstancedPropertiesGPUData::mTextureRegionSize);
    addInstancedProperty(MaterialInstancedPropertiesGPUData::mDepth);

    std::vector<GPUStructDefinition::GPUStructVariable> gpuStructVariableaArray;
    FOR_LIST(it, mInstancedProperties)
    {
        const GPUVariableData& gpuVariableData = *it;
        gpuStructVariableaArray.push_back({gpuVariableData.mGPUDataType, gpuVariableData.mName});
    }

    mInstancedPropertiesStructDefinition = 
    {
        "instancedPropertiesStruct",
        {
            gpuStructVariableaArray
        }
    };

    GPUDataType instancedPropertiesStructDataType = {mInstancedPropertiesStructDefinition.mName, mInstancedPropertiesStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};
    
    mInstancedPropertiesSharedBufferData = {
        GPUBufferType::STORAGE,
        {
            {{GPUStorage::UNIFORM, instancedPropertiesStructDataType, "instancedPropertiesArray"}, "", " "}
        },
        "InstancedProperties",
        "instancedProperties"
    };

    loadTextures();
}

void Material::enable() const
{
	PROFILER_CPU()

	if (mTextures[(u32)TextureType::BASE_COLOR])
	{
        mTextures[(u32)TextureType::BASE_COLOR]->enable();
	}
}

void Material::disable() const
{
	PROFILER_CPU()

	if (mTextures[(u32)TextureType::BASE_COLOR])
	{
        mTextures[(u32)TextureType::BASE_COLOR]->disable();
	}
}

bool Material::hasTexture() const
{
    return mTextures[(u32)TextureType::BASE_COLOR].isValid();
}

void Material::loadTextures()
{
    FOR_RANGE(i, 0, mMaterialData.mTexturePaths.size())
    {
        if(!mMaterialData.mTexturePaths[i].empty())
        {
            TextureData textureData;
            textureData.mPath = mMaterialData.mTexturePaths[i];
            textureData.mCreateMipMap = mMaterialData.mCreateMipMap;
                
            mTextures[i] = GET_SYSTEM(MaterialManager).loadTexture(textureData);
        }
    }
}

MaterialInstance Material::createMaterialInstance() const
{
    MaterialInstance instance;
    instance.mMaterial = getPtrToThis();
    instance.mMaterialInstancedProperties = mMaterialData.mMaterialInstancedProperties;
    return instance;
}

void MaterialFont::loadTextures()
{
    CHECK_MSG(!mMaterialData.mFontData.mPath.empty(), "mMaterialData.mFontData.mPath cannot be empty!");
    TextureData textureData;
    textureData.mPath = mMaterialData.mFontData.mPath;
    textureData.mCreateMipMap = mMaterialData.mCreateMipMap;
    textureData.mFontData = mMaterialData.mFontData;
    mTextures[(u32)TextureType::BASE_COLOR] = Ptr<const Texture>::cast(GET_SYSTEM(MaterialManager).loadTextureFont(textureData));
}

IMPLEMENT_SERIALIZATION(Material)
{
}

IMPLEMENT_DESERIALIZATION(Material)
{
}
