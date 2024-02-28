#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/Systems/System.hpp"

class Model;

class ModelManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    Ptr<const Model> loadModel(const std::string& path);

private:
    std::unordered_map<std::string, OwnerPtr<Model>> mModelMap;
};
REGISTER_CLASS(ModelManager);
