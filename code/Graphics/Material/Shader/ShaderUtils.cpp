#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Material.hpp"
#include "Assets/Config/Paths.hpp"
#include "Core/File/FileUtils.hpp"

OwnerPtr<GPUProgram> ShaderUtils::createShader(const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer, const Material& material)
{
	OwnerPtr<GPUProgram> shader = OwnerPtr<GPUProgram>::newObject();

    ShaderBuilder sbVert;
    ShaderBuilder sbFrag;
    material.createVertexShader(sbVert, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    material.createFragmentShader(sbFrag, gpuVertexBuffersContainer, gpuSharedBuffersContainer);

    std::string stringShderVert = sbVert.getCode();
    FileUtils::writeFile(Paths::mOutputShaders + std::to_string(material.getID()) + ".vs", [stringShderVert](std::ofstream& file)
    {
        file << stringShderVert;
    });
    std::string stringShderFrag = sbFrag.getCode();
    FileUtils::writeFile(Paths::mOutputShaders + std::to_string(material.getID()) + ".fs", [stringShderFrag](std::ofstream& file)
    {
        file << stringShderFrag;
    });
    shader->initFromFileContents(stringShderVert, stringShderFrag);
    return shader;
}
