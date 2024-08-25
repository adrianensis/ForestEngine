#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/GPU/GPUTexture.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/Model/Model.hpp"
#include "Core/Config/Paths.hpp"
#include "Core/File/FileUtils.hpp"

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
    registerBuffers();
}

void Shader::registerBuffers()
{
    GPUVertexBufferData bufferDataInstanceIDs(GPUBuiltIn::VertexInput::mInstanceID, 1);
    mShaderData.mGPUVertexBuffersContainer.addVertexBuffer(bufferDataInstanceIDs, false);

    GPUVertexBufferData bufferDataObjectIDs(GPUBuiltIn::VertexInput::mObjectID, 1);
    mShaderData.mGPUVertexBuffersContainer.addVertexBuffer(bufferDataObjectIDs, false);

    GPUVertexBufferData bufferDataMaterialInstanceIDs(GPUBuiltIn::VertexInput::mMaterialInstanceID, 1);
    mShaderData.mGPUVertexBuffersContainer.addVertexBuffer(bufferDataMaterialInstanceIDs, false);

    std::vector<GPUVariableData> gpuVertexInputBuffers;
    gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mPosition);
    FOR_RANGE(i, 0, GPUBuiltIn::VertexInput::mTextureCoords.size())
    {
        gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mTextureCoords.at(i));
    }
    gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mColor);
    gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mNormal);
    gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mBonesIDs);
    gpuVertexInputBuffers.push_back(GPUBuiltIn::VertexInput::mBonesWeights);
    FOR_ARRAY(i, gpuVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = gpuVertexInputBuffers[i];
        GPUVertexBufferData bufferData(gpuVariableData);
        mShaderData.mGPUVertexBuffersContainer.addVertexBuffer(bufferData, false);
    }
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
    gpuProgram->enable();

    u32 textureUnit = 0;
    FOR_MAP(it, mShaderData.mFramebufferBindings)
    {
        gpuProgram->bindUniformValue<i32>(GPUBuiltIn::Uniforms::getSampler(it->second.mSamplerName).mName, textureUnit);
        textureUnit++;
    }

    // Init all samplers to disable
    FOR_MAP(it, mShaderData.mTextures)
    {
        // NOTE: We reserve position 0 to represent NULL
        gpuProgram->bindUniformValue<u32>(GPUBuiltIn::Uniforms::getTextureHandler(*it).mName, 0);
    }

    FOR_MAP(it, textures)
    {
        // NOTE: We reserve position 0 to represent NULL
        gpuProgram->bindUniformValue<u32>(GPUBuiltIn::Uniforms::getTextureHandler(it->first).mName, it->second->getID() + 1);
    }

    gpuProgram->disable();
}

void Shader::addFramebufferBinding(const FramebufferBinding& framebufferBinding)
{
    mShaderData.mFramebufferBindings.insert_or_assign(framebufferBinding.mSamplerName, framebufferBinding);
}

void Shader::generateGPUProgramData(GPUProgramData& gpuProgramData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
}

OwnerPtr<GPUProgram> Shader::compileShader(GPURenderPass* vulkanRenderPass, HashedString label, HashedString id)
{
    OwnerPtr<GPUProgram> gpuProgram = OwnerPtr<GPUProgram>::newObject();

    ShaderBuilder sbVert;
    ShaderBuilder sbFrag;
    createVertexShader(sbVert, mShaderData.mGPUVertexBuffersContainer);
    createFragmentShader(sbFrag, mShaderData.mGPUVertexBuffersContainer);

    // if (!vertexShader->initialize(sbVert.getCode()/*fileSystem->readBytes("shaders/simple_shader.vert.spv")*/)) {
    //     CHECK_MSG(false, "Could not initialize vertex shader");
    //     // return false;
    // }
    // if (!fragmentShader->initialize(sbFrag.getCode()/*fileSystem->readBytes("shaders/simple_shader.frag.spv")*/)) {
    //     CHECK_MSG(false, "Could not initialize fragment shader");
    //     // return false;
    // }

    std::string stringShderVert = sbVert.getCode();
    FileUtils::writeFile(Paths::mOutputShaders.get() + id.get() + "_" + label.get() + ".vs", [stringShderVert](std::ofstream& file)
    {
        file << stringShderVert;
    });
    std::string stringShderFrag = sbFrag.getCode();
    FileUtils::writeFile(Paths::mOutputShaders.get() + id.get() + "_" + label.get() + ".fs", [stringShderFrag](std::ofstream& file)
    {
        file << stringShderFrag;
    });

    gpuProgram->initFromFileContents(vulkanRenderPass, GET_SYSTEM(GPUInstance).mGPUContext, stringShderVert, stringShderFrag);

    return gpuProgram;
}
