#include "GPU/Shader/GPUShader.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"

#include "GPU/Image/GPUTexture.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/GPUInstance.hpp"
#include "Engine/Paths.hpp"
#include "Core/File/FileUtils.hpp"

void GPUShaderPropertiesInstance::setDirty()
{
    PROFILER_CPU();
    GET_SYSTEM(GPUShaderManager).setGPUShaderPropertiesInstanceDirty(mID);
}

void GPUShader::init(const GPUShaderData& shaderData, const Core::GenericObjectBuffer& propertiesBlockGPUShaderDefault, Core::u32 id)
{
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
        GPUShaderPropertiesBlockNames::smPropertiesBlockInstanceName
    };

    mPropertiesBlockStructDefinition = propertiesBlockStructDefinition;
    mPropertiesBlockUniformBufferData = propertiesBlockUniformBufferData;

    registerTextures();
    setSharedGPUShaderPropertiesBlock();
    mSharedGPUShaderPropertiesBlockBuffer = propertiesBlockGPUShaderDefault;
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

bool GPUShader::hasFramebufferBinding(Core::HashedString bindingName) const
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

Core::OwnerPtr<GPUShaderPipeline> GPUShader::compileShader(const GPUShaderCompilationData& shaderCompilationData)
{
    PROFILER_CPU_NAMED(compileShader)

    mGPUShaderCompilationData = shaderCompilationData;

    std::vector<GPUShaderTextureBinding> gpuGPUShaderTextureBindings;
    const std::unordered_map<Core::HashedString, Core::WeakPtr<GPUTexture>> &shaderTextures = GET_SYSTEM(GPUShaderManager).getGPUShaderTextureBindings(getID());
    FOR_MAP(it, shaderTextures)
    {
        gpuGPUShaderTextureBindings.emplace_back(GPUShaderTextureBinding{it->first, it->second});
    }
    
    GPUShaderDescriptorSetsData gpuGPUShaderDescriptorSetsData
    {
        mGPUShaderCompilationData.mUniformBuffers,
        gpuGPUShaderTextureBindings
    };

    Core::OwnerPtr<GPUShaderPipeline> gpuGPUShaderPipeline = Core::OwnerPtr<GPUShaderPipeline>::newObject();
    GPUShaderPipelineData gpuGPUShaderPipelineData
    {
        gpuGPUShaderDescriptorSetsData,
        mGPUShaderCompilationData.mInputVertexBuffersContainer.getVertexBuffers(),
        shaderCompilationData.mGPUShaderPipelineDepthStencilData
    };
    gpuGPUShaderPipeline->init(gpuGPUShaderPipelineData, mGPUShaderCompilationData.mRenderPass, GET_SYSTEM(GPUInstance).mGPUContext);

    GPUShaderBuilder sbVert;
    GPUShaderBuilder sbFrag;
    {
        PROFILER_CPU_NAMED(create_vertex_and_fragment)
        createVertexShader(sbVert, mGPUShaderCompilationData.mInputVertexBuffersContainer, gpuGPUShaderPipeline->getGPUShaderDescriptorSets());
        createFragmentShader(sbFrag, mGPUShaderCompilationData.mInputVertexBuffersContainer, gpuGPUShaderPipeline->getGPUShaderDescriptorSets());
    }

    std::string stringGPUShaderVert = sbVert.getCode();
    std::string shaderPathVert = Core::Paths::mOutputShaders.get() + mGPUShaderCompilationData.id.get() + "_" + mGPUShaderCompilationData.label.get() + ".vert";
    Core::FileUtils::writeFile(shaderPathVert, [stringGPUShaderVert](std::ofstream& file)
    {
        file << stringGPUShaderVert;
    });

    std::string stringGPUShaderFrag = sbFrag.getCode();
    std::string shaderPathFrag = Core::Paths::mOutputShaders.get() + mGPUShaderCompilationData.id.get() + "_" + mGPUShaderCompilationData.label.get() + ".frag";
    Core::FileUtils::writeFile(shaderPathFrag, [stringGPUShaderFrag](std::ofstream& file)
    {
        file << stringGPUShaderFrag;
    });

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
    gpuGPUShaderPipeline->compile(vertexGPUShaderModuleData, fragmentGPUShaderModuleData);

    return gpuGPUShaderPipeline;
}