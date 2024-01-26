#include "Graphics/Shader/ShaderUtils.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Material.hpp"

OwnerPtr<GPUShader> ShaderUtils::createShader(const GPUBuffersLayout& gpuBuffersLayout, Ptr<const Material> material)
{
	OwnerPtr<GPUShader> shader = OwnerPtr<GPUShader>::newObject();
    ShaderBuilder sbVert;
    sbVert.createVertexShader(gpuBuffersLayout, material);
    ShaderBuilder sbFrag;
    sbFrag.createFragmentShader(gpuBuffersLayout, material);

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
