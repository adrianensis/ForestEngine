#pragma once

#include "Core/Module.hpp"

class Model;

class ModelManager: public ObjectBase, public Singleton<ModelManager>
{
	GENERATE_METADATA(ModelManager)

public:
    void init();
    void terminate();

    Ptr<const Model> loadModel(const std::string& path);

private:
    std::map<std::string, OwnerPtr<Model>> mModelMap;
};
