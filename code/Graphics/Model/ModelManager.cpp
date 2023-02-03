#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/Model/Model.hpp"

void ModelManager::init()
{

}

void ModelManager::terminate()
{
    mModelMap.clear();
}

Ptr<const Model> ModelManager::loadModel(const std::string& path)
{
    if (!MAP_CONTAINS(mModelMap, path))
	{
		OwnerPtr<Model> model = OwnerPtr<Model>(NEW(Model));
		model.get().init(path);
		MAP_INSERT(mModelMap, path, model);
	}

	return mModelMap.at(path);
}
