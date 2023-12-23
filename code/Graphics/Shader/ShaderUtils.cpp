#include "Graphics/Shader/ShaderUtils.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilder.hpp"

OwnerPtr<GPUShader> ShaderUtils::createShader(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material)
{
	OwnerPtr<GPUShader> shader = OwnerPtr<GPUShader>::newObject();
    ShaderBuilder sbVert;
    sbVert.createVertexShader(gpuVertexBuffersLayout, material);
    ShaderBuilder sbFrag;
    sbFrag.createFragmentShader(gpuVertexBuffersLayout, material);

    std::string stringShderVert = sbVert.getCode();
    LOG(stringShderVert);
    std::string stringShderFrag = sbFrag.getCode();
    LOG(stringShderFrag);
    shader->initFromFileContents(stringShderVert, stringShderFrag);
    return shader;
}
