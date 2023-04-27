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
		MAP_INSERT(mModelMap, path, SharedPtr<Model>::newObject());
		mModelMap.at(path)->init(path);
	}

	return mModelMap.at(path);
}
