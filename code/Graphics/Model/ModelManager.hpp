#pragma once

#include "Core/Module.hpp"
#include "Core/Systems/System.hpp"

class Model;

class ModelManager: public System
{
	

public:
    void init();
    void terminate();

    Ptr<const Model> loadModel(const std::string& path);

private:
    std::unordered_map<std::string, OwnerPtr<Model>> mModelMap;
};
REGISTER_CLASS(ModelManager);
