#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

#include "GPU/Image/GPUTexture.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/GPUInstance.hpp"
#include "Graphics/Model/Model.hpp"
#include "Core/Config/Paths.hpp"
#include "Core/File/FileUtils.hpp"
#include <cstdlib>

void ShaderPropertiesInstance::setDirty()
{
    PROFILER_CPU();
    GET_SYSTEM(ShaderManager).setShaderPropertiesInstanceDirty(mID);
}

void Shader::init(const ShaderData& shaderData, const GenericObjectBuffer& propertiesBlockShaderDefault, u32 id)
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
    setSharedShaderPropertiesBlock();
    mSharedShaderPropertiesBlockBuffer = propertiesBlockShaderDefault;
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

OwnerPtr<GPUShaderPipeline> Shader::compileShader(const ShaderCompilationData& shaderCompilationData)
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

    OwnerPtr<GPUShaderPipeline> gpuShaderPipeline = OwnerPtr<GPUShaderPipeline>::newObject();
    GPUShaderPipelineData gpuShaderPipelineData
    {
        gpuShaderDescriptorSetsData,
        mShaderCompilationData.mInputVertexBuffersContainer.getVertexBuffers(),
        shaderCompilationData.mGPUShaderPipelineDepthStencilData
    };
    gpuShaderPipeline->init(gpuShaderPipelineData, mShaderCompilationData.mRenderPass, GET_SYSTEM(GPUInstance).mGPUContext);

    ShaderBuilder sbVert;
    ShaderBuilder sbFrag;
    {
        PROFILER_CPU_NAMED(create_vertex_and_fragment)
        createVertexShader(sbVert, mShaderCompilationData.mInputVertexBuffersContainer, gpuShaderPipeline->getGPUShaderDescriptorSets());
        createFragmentShader(sbFrag, mShaderCompilationData.mInputVertexBuffersContainer, gpuShaderPipeline->getGPUShaderDescriptorSets());
    }

    std::string stringShaderVert = sbVert.getCode();
    std::string shaderPathVert = Paths::mOutputShaders.get() + mShaderCompilationData.id.get() + "_" + mShaderCompilationData.label.get() + ".vert";
    FileUtils::writeFile(shaderPathVert, [stringShaderVert](std::ofstream& file)
    {
        file << stringShaderVert;
    });

    std::string stringShaderFrag = sbFrag.getCode();
    std::string shaderPathFrag = Paths::mOutputShaders.get() + mShaderCompilationData.id.get() + "_" + mShaderCompilationData.label.get() + ".frag";
    FileUtils::writeFile(shaderPathFrag, [stringShaderFrag](std::ofstream& file)
    {
        file << stringShaderFrag;
    });

    GPUShaderModuleData vertexGPUShaderModuleData
    {
        GPUPipelineStage::VERTEX,
        stringShaderVert,
        shaderCompilationData.id
    };
    GPUShaderModuleData fragmentGPUShaderModuleData
    {
        GPUPipelineStage::FRAGMENT,
        stringShaderFrag,
        shaderCompilationData.id
    };
    gpuShaderPipeline->compile(vertexGPUShaderModuleData, fragmentGPUShaderModuleData);

    return gpuShaderPipeline;
}