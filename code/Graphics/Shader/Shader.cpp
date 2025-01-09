#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

#include "GPU/Image/GPUTexture.hpp"
#include "GPU/Shader/GPUShader.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/GPUInstance.hpp"
#include "Graphics/Model/Model.hpp"
#include "Core/Config/Paths.hpp"
#include "Core/File/FileUtils.hpp"
#include <cstdlib>

void Shader::init(const ShaderData& shaderData, u32 id)
{
    mShaderData = shaderData;
	mID = id;

    GPUStructDefinition propertiesBlockStructDefinition =
    {
        ShaderPropertiesBlockNames::smPropertiesBlockStructName,
        {
            generateShaderPropertiesBlock()
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

    mPropertiesBlockStructDefinition = propertiesBlockStructDefinition;
    mPropertiesBlockUniformBufferData = propertiesBlockUniformBufferData;

    registerTextures();
}

void Shader::terminate()
{

}

std::vector<GPUStructDefinition::GPUStructVariable> Shader::generateShaderPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock =
    {
        {GPUShaderDefinitions::PrimitiveTypes::mInt, "_emptyStructFixHack"}
    };

    return propertiesBlock;
}

void Shader::enable() const
{
	PROFILER_CPU()
    u32 textureUnit = 0;
    FOR_MAP(it, mFramebufferBindings)
    {
//        GET_SYSTEM(GPUInterface).enableTexture(it->second.mTextureID, textureUnit, it->second.mStage);
        textureUnit++;
    }
}

void Shader::disable() const
{
	PROFILER_CPU()
    u32 textureUnit = 0;
    FOR_MAP(it, mFramebufferBindings)
    {
//        GET_SYSTEM(GPUInterface).disableTexture(textureUnit, it->second.mStage);
        textureUnit++;
    }
}

bool Shader::hasFramebufferBinding(HashedString bindingName) const
{
    return mFramebufferBindings.contains(bindingName);
}

void Shader::addFramebufferBinding(const FramebufferBinding& framebufferBinding)
{
    mFramebufferBindings.insert_or_assign(framebufferBinding.mSamplerName, framebufferBinding);
}

void Shader::generateShaderGenerationData(ShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
}

OwnerPtr<GPUShader> Shader::compileShader(const ShaderCompilationData& shaderCompilationData)
{
    PROFILER_CPU_NAMED(compileShader)

    mShaderCompilationData = shaderCompilationData;

    std::vector<GPUShaderTextureBinding> gpuShaderTextureBindings;
    const std::unordered_map<HashedString, WeakPtr<GPUTexture>> &shaderTextures = GET_SYSTEM(ShaderManager).getShaderTextureBindings(getID());
    FOR_MAP(it, shaderTextures)
    {
        gpuShaderTextureBindings.emplace_back(GPUShaderTextureBinding{it->first, it->second});
    }
    
    GPUShaderDescriptorSetsData gpuShaderDescriptorSetsData
    {
        mShaderCompilationData.mUniformBuffers,
        gpuShaderTextureBindings
    };

    OwnerPtr<GPUShader> gpuShader = OwnerPtr<GPUShader>::newObject();
    gpuShader->init(mShaderCompilationData.vulkanRenderPass, gpuShaderDescriptorSetsData, mShaderCompilationData.mInputVertexBuffersContainer.getVertexBuffers(), GET_SYSTEM(GPUInstance).mGPUContext);

    ShaderBuilder sbVert;
    ShaderBuilder sbFrag;
    createVertexShader(sbVert, mShaderCompilationData.mInputVertexBuffersContainer, gpuShader->getGPUShaderDescriptorSets());
    createFragmentShader(sbFrag, mShaderCompilationData.mInputVertexBuffersContainer, gpuShader->getGPUShaderDescriptorSets());

    std::string stringShaderVert = sbVert.getCode();
    std::string shaderPathVert = Paths::mOutputShaders.get() + mShaderCompilationData.id.get() + "_" + mShaderCompilationData.label.get() + ".vert";
    FileUtils::writeFile(shaderPathVert, [stringShaderVert](std::ofstream& file)
    {
        file << stringShaderVert;
    });

    system(std::string("glslc "s + shaderPathVert + " -o "s + shaderPathVert + ".spv").c_str());
    
    std::vector<byte> stringShaderVertSpvBinary;
    FileUtils::readFileBinaryData(shaderPathVert + ".spv", stringShaderVertSpvBinary);

    std::string stringShaderFrag = sbFrag.getCode();
    std::string shaderPathFrag = Paths::mOutputShaders.get() + mShaderCompilationData.id.get() + "_" + mShaderCompilationData.label.get() + ".frag";
    FileUtils::writeFile(shaderPathFrag, [stringShaderFrag](std::ofstream& file)
    {
        file << stringShaderFrag;
    });

    system(std::string("glslc "s + shaderPathFrag + " -o "s + shaderPathFrag + ".spv").c_str());
    
    std::vector<byte> stringShaderFragSpvBinary;
    FileUtils::readFileBinaryData(shaderPathFrag + ".spv", stringShaderFragSpvBinary);

    gpuShader->compile(stringShaderVertSpvBinary, stringShaderFragSpvBinary);

    return gpuShader;
}