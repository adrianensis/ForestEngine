#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Material/Shader/ShaderPBR.hpp"

void ModelManager::init()
{
    MaterialData materialData;
    materialData.mMaxInstances = 500;
    materialData.setSharedMaterialPropertiesBlock<MetallicRoughness>();
    // pink
    materialData.mSharedMaterialPropertiesBlockBuffer.get<MetallicRoughness>().mBaseColor = Vector4(255.0f/256.0f,20.0f/256.0f,147.0f/256.0f,1);
    mDefaultModelMaterial = GET_SYSTEM(MaterialManager).createMaterial<ShaderPBR>(materialData);
}

void ModelManager::terminate()
{
    mModels.clear();
}
void ModelManager::setMeshToModel(Ptr<const GPUMesh> mesh, Ptr<Model> model)
{
    mMeshToModels.insert_or_assign(mesh, model);
}

Ptr<Model> ModelManager::getModelFromMesh(Ptr<const GPUMesh> mesh) const
{
    if(!mMeshToModels.contains(mesh))
    {
        return Ptr<Model>();
    }

    return mMeshToModels.at(mesh);
}

Ptr<const Model> ModelManager::loadModel(const std::string& path)
{
    if (!mModels.contains(path))
	{
		mModels.insert_or_assign(path, OwnerPtr<Model>::newObject());
		mModels.at(path)->init(path);
	}

	return mModels.at(path);
}
