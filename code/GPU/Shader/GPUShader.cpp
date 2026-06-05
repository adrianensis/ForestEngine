#include "GPU/Shader/GPUShader.hpp"
#include "GPU/Core/GPUDefinitions.h"
#include "GPU/Shader/GPUShaderManager.hpp"

#include "GPU/Texture/GPUTexture.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"

void GPUShaderPropertiesInstance::setDirty()
{
    PROFILER_CPU();
    mShader->mGPUShaderManager->setGPUShaderPropertiesInstanceDirty(mID);
}

void GPUShader::init(GPUContext* gpuContext, GPUShaderManager* gpuShaderManager, const GPUShaderData& shaderData, const GPU::GenericObjectBuffer& propertiesBlockGPUShader, GPU::u32 id)
{
    mGPUContext = gpuContext; 
    mGPUShaderManager = gpuShaderManager; 
    mGPUShaderData = shaderData;
	mID = id;

    GPUStructDefinition propertiesBlockStructDefinition =
    {
        GPUShaderPropertiesBlockNames::smPropertiesBlockStructName,
        {
            generateGPUShaderPropertiesBlock()
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
            {{GPUStorage::UNIFORM, propertiesBlockStructDataType, GPUShaderPropertiesBlockNames::smPropertiesBlockArrayName}, "", " "}
        },
        GPUShaderPropertiesBlockNames::smPropertiesBlockBufferName,
        GPUShaderPropertiesBlockNames::smPropertiesBlockInstanceName,
        GPUDescriptorSetScope::LOCAL
    };

    mPropertiesBlockStructDefinition = propertiesBlockStructDefinition;
    mPropertiesBlockUniformBufferData = propertiesBlockUniformBufferData;

    registerTextures();
    setSharedGPUShaderPropertiesBlock();
    mSharedGPUShaderPropertiesBlockBuffer = propertiesBlockGPUShader;
}

void GPUShader::terminate()
{

}

std::vector<GPUStructDefinition::GPUStructVariable> GPUShader::generateGPUShaderPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock =
    {
        {GPUShaderDefinitions::PrimitiveTypes::mInt, "_emptyStructFixHack"}
    };

    return propertiesBlock;
}

bool GPUShader::hasFramebufferBinding(std::string bindingName) const
{
    return mFramebufferBindings.contains(bindingName);
}

void GPUShader::addFramebufferBinding(const FramebufferBinding& framebufferBinding)
{
    mFramebufferBindings.insert_or_assign(framebufferBinding.mSamplerName, framebufferBinding);
}

void GPUShader::generateGPUShaderGenerationData(GPUShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
}

GPUShaderPipeline* GPUShader::compileShader(const GPUShaderCompilationData& shaderCompilationData)
{
    PROFILER_CPU_NAMED(compileShader)

    // TODO: Refactor this method to external ShaderUtils?

    GPUShaderPipeline* gpuShaderPipeline = new  GPUShaderPipeline();
    GPUShaderPipelineData gpuShaderPipelineData
    {
        shaderCompilationData.mGPUDescriptorSetGlobal,
        shaderCompilationData.mGPUDescriptorSetLocal,
        shaderCompilationData.mInputVertexBuffersContainer.getVertexBuffers(),
    };
    gpuShaderPipeline->init(gpuShaderPipelineData, mGPUContext);

    GPUShaderBuilder sbVert;
    GPUShaderBuilder sbFrag;
    {
        PROFILER_CPU_NAMED(create_vertex_and_fragment)
        createVertexShader(sbVert, shaderCompilationData);
        createFragmentShader(sbFrag, shaderCompilationData);
    }

    // TODO: refactor std::string("output/shaders/")
    
    std::string stringGPUShaderVert = sbVert.getCode();
    std::string shaderPathVert = std::string("output/shaders/") + shaderCompilationData.id + "_" + shaderCompilationData.label + ".vert";
    writeShader(shaderPathVert, stringGPUShaderVert);

    std::string stringGPUShaderFrag = sbFrag.getCode();
    std::string shaderPathFrag = std::string("output/shaders/") + shaderCompilationData.id + "_" + shaderCompilationData.label + ".frag";
    writeShader(shaderPathFrag, stringGPUShaderFrag);

    GPUShaderModuleData vertexGPUShaderModuleData
    {
        GPUPipelineStage::VERTEX,
        stringGPUShaderVert,
        shaderCompilationData.id
    };
    GPUShaderModuleData fragmentGPUShaderModuleData
    {
        GPUPipelineStage::FRAGMENT,
        stringGPUShaderFrag,
        shaderCompilationData.id
    };

    // TODO: refactor shader pipeline into shader pipeline manager (map: key = descriptor set + InputVertexBuffers)
    gpuShaderPipeline->compile(vertexGPUShaderModuleData, fragmentGPUShaderModuleData);

    return gpuShaderPipeline;
}

void GPUShader::writeShader(const std::string& path, const std::string& shaderContent)
{
    const std::filesystem::path fsPath{path};
    const std::filesystem::path fileName = fsPath.filename();
    CHECK_MSG(fsPath.has_filename(), "No filename found in " + path);

    if(!std::filesystem::exists(fsPath.parent_path()))
    {
        std::filesystem::create_directories(fsPath.parent_path());
    }

    std::ofstream file;
    file.open(path);
    CHECK_MSG(file.good() && !file.fail(), "Couldn't open " + path);

    file << shaderContent;
    file.close();
}