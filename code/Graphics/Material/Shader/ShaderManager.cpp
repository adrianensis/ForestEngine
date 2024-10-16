#include "Graphics/Material/Shader/ShaderManager.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Material.hpp"
#include "Core/Config/Paths.hpp"
#include "Core/File/FileUtils.hpp"

void ShaderManager::init()
{
}

void ShaderManager::terminate()
{
    mShaders.clear();
}

OwnerPtr<GPUProgram> ShaderManager::compileShader(HashedString label, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const Material& material, Ptr<const Shader> customShader)
{
    OwnerPtr<GPUProgram> gpuProgram = OwnerPtr<GPUProgram>::newObject();

    ShaderBuilder sbVert;
    ShaderBuilder sbFrag;
    customShader->createVertexShader(sbVert, gpuVertexBuffersContainer);
    customShader->createFragmentShader(sbFrag, gpuVertexBuffersContainer);

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