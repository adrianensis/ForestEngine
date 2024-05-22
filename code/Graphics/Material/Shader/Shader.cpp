#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/RenderSharedContext.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Model.hpp"

void Shader::init(PoolHandler<Material> material)
{
    mShaderData.mMaterial = material;

    GPUStructDefinition propertiesBlockStructDefinition =
    {
        "propertiesBlockStruct",
        {
            generateMaterialPropertiesBlock()
        }
    };

    GPUDataType propertiesBlockStructDataType =
    {
        propertiesBlockStructDefinition.mName,
        propertiesBlockStructDefinition.getTypeSizeInBytes(),
        GPUPrimitiveDataType::STRUCT
    };
    
    GPUSharedBufferData propertiesBlockSharedBufferData =
    {
        GPUBufferType::STORAGE,
        {
            {{GPUStorage::UNIFORM, propertiesBlockStructDataType, "propertiesBlockArray"}, "", " "}
        },
        "PropertiesBlock",
        "propertiesBlock"
    };

    mShaderData.mPropertiesBlockStructDefinition = propertiesBlockStructDefinition;
    mShaderData.mPropertiesBlockSharedBufferData = propertiesBlockSharedBufferData;

    loadTextures();
}

void Shader::init(const ShaderData& shaderData)
{
    mShaderData = shaderData;
}

void Shader::terminate()
{
    FOR_MAP(it, mShaderData.mTextures)
    {
        it->second.reset();
    }
}

std::vector<GPUStructDefinition::GPUStructVariable> Shader::generateMaterialPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {GPUBuiltIn::PrimitiveTypes::mInt, "_emptyStructFixHack"}
    };

    return propertiesBlock;
}

void Shader::enable() const
{
	PROFILER_CPU()
    u32 textureUnit = 0;
    FOR_MAP(it, mShaderData.mTextures)
    {
        mShaderData.mTextures.at(it->first).get().enable(textureUnit);
        textureUnit++;
    }
}

void Shader::disable() const
{
	PROFILER_CPU()
    FOR_MAP(it, mShaderData.mTextures)
    {
        mShaderData.mTextures.at(it->first).get().disable();
    }
}

bool Shader::hasTexture(ConstString bindingName) const
{
    return mShaderData.mTextures.contains(bindingName) && mShaderData.mTextures.at(bindingName).isValid();
}

void Shader::bindTextures(Ptr<GPUProgram> gpuProgram) const
{
    gpuProgram->enable();

    u32 textureUnit = 0;
    FOR_MAP(it, mShaderData.mTextures)
    {
        gpuProgram->bindUniformValue<i32>(GPUBuiltIn::Uniforms::getSampler(it->first).mName, textureUnit);
        textureUnit++;
    }
    
    gpuProgram->disable();
}

void Shader::loadTextures()
{
    FOR_MAP(it, mShaderData.mMaterial->getMaterialData().mTextureBindings)
    {
        CHECK_MSG(!it->second.mPath.empty(), "texture mPath cannot be empty!");
        TextureData textureData;
        textureData.mPath = it->second.mPath;
        textureData.mStage = it->second.mStage;

        if(mShaderData.mMaterial->getMaterialData().mIsFont)
        {
            CHECK_MSG(!mShaderData.mMaterial->getMaterialData().mFontData.mPath.empty(), "mMaterialData.mFontData.mPath cannot be empty!");
            textureData.mIsFont = true;
            textureData.mFontData = mShaderData.mMaterial->getMaterialData().mFontData;
        }

        mShaderData.mTextures.insert_or_assign(it->first, GET_SYSTEM(MaterialManager).loadTexture(textureData));
    }
}

