#pragma once

#include "Engine/Core.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "Graphics/Model/Model.hpp"

class ModelManager: public System::System
{
public:
    void init();
    virtual void terminate() override;

    Core::WeakPtr<const Model> loadModel(const std::string& path);
    void setMeshToModel(Core::WeakPtr<const GPUMesh> mesh, Model* model);
    Model* getModelFromMesh(Core::WeakPtr<const GPUMesh> mesh) const;

private:
    std::unordered_map<std::string, Core::OwnerPtr<Model>> mModels;
    std::unordered_map<Core::WeakPtr<const GPUMesh>, Model*> mMeshToModels;
    Core::WeakPtr<GPUShader> mDefaultModelShader;
public:
    GET(DefaultModelShader)
    CRGET(Models)
    CRGET(MeshToModels)
};
REGISTER_CLASS(ModelManager, System);
