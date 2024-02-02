#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Material.hpp"

OwnerPtr<GPUProgram> ShaderUtils::createShader(const GPUBuffersContainer& gpuBuffersContainer, Ptr<const Material> material)
{
	OwnerPtr<GPUProgram> shader = OwnerPtr<GPUProgram>::newObject();

    ShaderBuilder sbVert;
    ShaderBuilder sbFrag;
    material->createVertexShader(sbVert, gpuBuffersContainer);
    material->createFragmentShader(sbFrag, gpuBuffersContainer);

    std::string stringShderVert = sbVert.getCode();
    FileUtils::writeFile(Paths::mOutputShaders + std::to_string(material->getID()) + ".vs", [stringShderVert](std::ofstream& file)
    {
        file << stringShderVert;
    });
    std::string stringShderFrag = sbFrag.getCode();
    FileUtils::writeFile(Paths::mOutputShaders + std::to_string(material->getID()) + ".fs", [stringShderFrag](std::ofstream& file)
    {
        file << stringShderFrag;
    });
    shader->initFromFileContents(stringShderVert, stringShderFrag);
    return shader;
}
