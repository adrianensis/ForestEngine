#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/Model/Model.hpp"

void ModelManager::init()
{

}

void ModelManager::terminate()
{
    mModels.clear();
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
