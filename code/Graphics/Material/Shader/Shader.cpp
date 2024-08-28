#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/GPU/GPUTexture.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/Mesh/GPUMesh.hpp"
#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/Model/Model.hpp"
#include "Core/Config/Paths.hpp"
#include "Core/File/FileUtils.hpp"
#include <cstdlib>

void Shader::init()
{

    GPUStructDefinition propertiesBlockStructDefinition =
    {
        ShaderPropertiesBlockNames::smPropertiesBlockStructName,
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

    GPUUniformBufferData propertiesBlockUniformBufferData =
    {
        GPUBufferType::STORAGE,
        {
            {{GPUStorage::UNIFORM, propertiesBlockStructDataType, ShaderPropertiesBlockNames::smPropertiesBlockArrayName}, "", " "}
        },
        ShaderPropertiesBlockNames::smPropertiesBlockBufferName,
        ShaderPropertiesBlockNames::smPropertiesBlockInstanceName
    };

    mShaderData.mPropertiesBlockStructDefinition = propertiesBlockStructDefinition;
    mShaderData.mPropertiesBlockUniformBufferData = propertiesBlockUniformBufferData;

    registerTextures();
}

void Shader::init(const ShaderData& shaderData)
{
    mShaderData = shaderData;
}

void Shader::terminate()
{

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
    FOR_MAP(it, mShaderData.mFramebufferBindings)
    {
//        GET_SYSTEM(GPUInterface).enableTexture(it->second.mTextureID, textureUnit, it->second.mStage);
        textureUnit++;
    }
}

void Shader::disable() const
{
	PROFILER_CPU()
    u32 textureUnit = 0;
    FOR_MAP(it, mShaderData.mFramebufferBindings)
    {
//        GET_SYSTEM(GPUInterface).disableTexture(textureUnit, it->second.mStage);
        textureUnit++;
    }
}

bool Shader::hasFramebufferBinding(HashedString bindingName) const
{
    return mShaderData.mFramebufferBindings.contains(bindingName);
}

void Shader::bindTextures(Ptr<GPUProgram> gpuProgram, const std::unordered_map<HashedString, PoolHandler<GPUTexture>>& textures) const
{
    // gpuProgram->enable();

    // u32 textureUnit = 0;
    // FOR_MAP(it, mShaderData.mFramebufferBindings)
    // {
    //     gpuProgram->bindUniformValue<i32>(GPUBuiltIn::Uniforms::getSampler(it->second.mSamplerName).mName, textureUnit);
    //     textureUnit++;
    // }

    // // Init all samplers to disable
    // FOR_MAP(it, mShaderData.mTextures)
    // {
    //     // NOTE: We reserve position 0 to represent NULL
    //     gpuProgram->bindUniformValue<u32>(GPUBuiltIn::Uniforms::getTextureHandler(*it).mName, 0);
    // }

    // FOR_MAP(it, textures)
    // {
    //     // NOTE: We reserve position 0 to represent NULL
    //     gpuProgram->bindUniformValue<u32>(GPUBuiltIn::Uniforms::getTextureHandler(it->first).mName, it->second->getID() + 1);
    // }

    // gpuProgram->disable();
}

void Shader::addFramebufferBinding(const FramebufferBinding& framebufferBinding)
{
    mShaderData.mFramebufferBindings.insert_or_assign(framebufferBinding.mSamplerName, framebufferBinding);
}

void Shader::generateShaderGenerationData(ShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
}

OwnerPtr<GPUProgram> Shader::compileShader(const ShaderCompileData& shaderCompileData)
{
    mShaderData.mShaderCompileData = shaderCompileData;
    OwnerPtr<GPUProgram> gpuProgram = OwnerPtr<GPUProgram>::newObject();

    GPUVertexBuffersContainer gpuVertexBuffersContainer;
    shaderCompileData.mMesh->populateGPUVertexBuffersContainer(gpuVertexBuffersContainer, false); 

    ShaderBuilder sbVert;
    ShaderBuilder sbFrag;
    createVertexShader(sbVert, gpuVertexBuffersContainer);
    createFragmentShader(sbFrag, gpuVertexBuffersContainer);

    std::string stringShaderVert = sbVert.getCode();
    std::string shaderPathVert = Paths::mOutputShaders.get() + shaderCompileData.id.get() + "_" + shaderCompileData.label.get() + ".vert";
    FileUtils::writeFile(shaderPathVert, [stringShaderVert](std::ofstream& file)
    {
        file << stringShaderVert;
    });

    system(std::string("glslc "s + shaderPathVert + " -o "s + shaderPathVert + ".spv").c_str());
    
    std::vector<byte> stringShaderVertSpvBinary;
    FileUtils::readFileBinaryData(shaderPathVert + ".spv", stringShaderVertSpvBinary);

    std::string stringShaderFrag = sbFrag.getCode();
    std::string shaderPathFrag = Paths::mOutputShaders.get() + shaderCompileData.id.get() + "_" + shaderCompileData.label.get() + ".frag";
    FileUtils::writeFile(shaderPathFrag, [stringShaderFrag](std::ofstream& file)
    {
        file << stringShaderFrag;
    });

    system(std::string("glslc "s + shaderPathFrag + " -o "s + shaderPathFrag + ".spv").c_str());
    
    std::vector<byte> stringShaderFragSpvBinary;
    FileUtils::readFileBinaryData(shaderPathFrag + ".spv", stringShaderFragSpvBinary);

    gpuProgram->initFromFileContents(shaderCompileData.vulkanRenderPass, shaderCompileData.mUniformBuffers, gpuVertexBuffersContainer.getVertexBuffers(), GET_SYSTEM(GPUInstance).mGPUContext, stringShaderVertSpvBinary, stringShaderFragSpvBinary);

    return gpuProgram;
}
