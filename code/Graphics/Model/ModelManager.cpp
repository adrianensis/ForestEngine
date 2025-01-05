#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Material/Shader/ShaderPBR.hpp"

void ModelManager::init()
{
    MaterialData materialData;
    materialData.mMaxInstances = 500;
    materialData.setSharedMaterialPropertiesBlock<PropertiesBlockShaderPBR>();
    // pink
    materialData.mSharedMaterialPropertiesBlockBuffer.get<PropertiesBlockShaderPBR>().mBaseColor = Vector4(255.0f/256.0f,20.0f/256.0f,147.0f/256.0f,1);
    mDefaultModelMaterial = GET_SYSTEM(MaterialManager).createMaterial<ShaderDefault>(materialData);
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
        LOG_TRACE()
        PROFILER_CPU()
		mModels.insert_or_assign(path, OwnerPtr<Model>::newObject());
		mModels.at(path)->init(path);
	}

	return mModels.at(path);
}
