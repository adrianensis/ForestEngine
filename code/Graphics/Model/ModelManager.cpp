#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/Model/Model.hpp"

void ModelManager::init()
{

}

void ModelManager::terminate()
{
    mModelMap.clear();
}

Ptr<const Model> ModelManager::loadModel(CR(std::string) path)
{
    if (!MAP_CONTAINS(mModelMap, path))
	{
		OwnerPtr<Model> model = OwnerPtr<Model>(NEW(Model));
		model.get().init(path);
		MAP_INSERT(mModelMap, path, model);
	}

	return OwnerPtr<const Model>(mModelMap.at(path));
}
