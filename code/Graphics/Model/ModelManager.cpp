#include "Graphics/Model/ModelManager.hpp"
#include "GPU/Shader/BuiltIn/GPUShaderPBR.hpp"
#include "GPU/GPUInstance.hpp"

void ModelManager::init()
{
    GPUShaderData shaderData;
    shaderData.mMaxInstances = 500;
    PropertiesBlockGPUShaderDefault propertiesBlockGPUShaderDefault;
    // pink
    propertiesBlockGPUShaderDefault.mBaseColor = Maths::Vector4(255.0f/256.0f,20.0f/256.0f,147.0f/256.0f,1);
    mDefaultModelShader = GPUInstance::getInstance().mGPUShaderManager->createShader<GPUShaderDefault, PropertiesBlockGPUShaderDefault>(GPUInstance::getInstance().mGPUContext, shaderData, propertiesBlockGPUShaderDefault);
}

void ModelManager::terminate()
{
    mModels.clear();
}
void ModelManager::setMeshToModel(Core::WeakPtr<const GPUMesh> mesh, Core::WeakPtr<Model> model)
{
    mMeshToModels.insert_or_assign(mesh, model);
}

Core::WeakPtr<Model> ModelManager::getModelFromMesh(Core::WeakPtr<const GPUMesh> mesh) const
{
    if(!mMeshToModels.contains(mesh))
    {
        return Core::WeakPtr<Model>();
    }

    return mMeshToModels.at(mesh);
}

Core::WeakPtr<const Model> ModelManager::loadModel(const std::string& path)
{
    if (!mModels.contains(path))
	{
        PROFILER_CPU()
		mModels.insert_or_assign(path, Core::OwnerPtr<Model>::newObject());
		mModels.at(path)->init(path);
	}

	return mModels.at(path);
}
