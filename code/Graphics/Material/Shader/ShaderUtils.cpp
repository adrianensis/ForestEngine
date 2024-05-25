#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Material.hpp"
#include "Engine/Config/Paths.hpp"
#include "Core/File/FileUtils.hpp"

OwnerPtr<GPUProgram> ShaderUtils::createShader(ConstString label, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer, const Material& material)
{
	return ShaderUtils::createShaderCustomFragment(label, gpuVertexBuffersContainer, gpuSharedBuffersContainer, material, material.getShader());
}

OwnerPtr<GPUProgram> ShaderUtils::createShaderCustomFragment(ConstString label, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer, const Material& material, Ptr<const Shader> customShader)
{
	OwnerPtr<GPUProgram> gpuProgram = OwnerPtr<GPUProgram>::newObject();

    ShaderBuilder sbVert;
    ShaderBuilder sbFrag;
    // material.getShader()->createVertexShader(sbVert, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    customShader->createVertexShader(sbVert, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    customShader->createFragmentShader(sbFrag, gpuVertexBuffersContainer, gpuSharedBuffersContainer);

    std::string stringShderVert = sbVert.getCode();
    FileUtils::writeFile(Paths::mOutputShaders.get() + std::to_string(material.getID()) + "_" + label.get() + ".vs", [stringShderVert](std::ofstream& file)
    {
        file << stringShderVert;
    });
    std::string stringShderFrag = sbFrag.getCode();
    FileUtils::writeFile(Paths::mOutputShaders.get() + std::to_string(material.getID()) + "_" + label.get() + ".fs", [stringShderFrag](std::ofstream& file)
    {
        file << stringShderFrag;
    });
    gpuProgram->initFromFileContents(stringShderVert, stringShderFrag);
    return gpuProgram;
}
