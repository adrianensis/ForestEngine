#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/Model/Model.hpp"
#include "GPU/Shader/BuiltIn/GPUShaderPBR.hpp"

void ModelManager::init()
{
    GPUShaderData shaderData;
    shaderData.mMaxInstances = 500;
    PropertiesBlockGPUShaderDefault propertiesBlockGPUShaderDefault;
    // pink
    propertiesBlockGPUShaderDefault.mBaseColor = Vector4(255.0f/256.0f,20.0f/256.0f,147.0f/256.0f,1);
    mDefaultModelShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderDefault, PropertiesBlockGPUShaderDefault>(shaderData, propertiesBlockGPUShaderDefault);
}

void ModelManager::terminate()
{
    mModels.clear();
}
void ModelManager::setMeshToModel(WeakPtr<const GPUMesh> mesh, WeakPtr<Model> model)
{
    mMeshToModels.insert_or_assign(mesh, model);
}

WeakPtr<Model> ModelManager::getModelFromMesh(WeakPtr<const GPUMesh> mesh) const
{
    if(!mMeshToModels.contains(mesh))
    {
        return WeakPtr<Model>();
    }

    return mMeshToModels.at(mesh);
}

WeakPtr<const Model> ModelManager::loadModel(const std::string& path)
{
    if (!mModels.contains(path))
	{
        PROFILER_CPU()
		mModels.insert_or_assign(path, OwnerPtr<Model>::newObject());
		mModels.at(path)->init(path);
	}

	return mModels.at(path);
}
