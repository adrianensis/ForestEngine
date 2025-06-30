#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/Mesh/GPUMesh.hpp"

class Model;

class ModelManager: public Core::System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    Core::WeakPtr<const Model> loadModel(const std::string& path);
    void setMeshToModel(Core::WeakPtr<const GPUMesh> mesh, Core::WeakPtr<Model> model);
    Core::WeakPtr<Model> getModelFromMesh(Core::WeakPtr<const GPUMesh> mesh) const;

private:
    std::unordered_map<std::string, Core::OwnerPtr<Model>> mModels;
    std::unordered_map<Core::WeakPtr<const GPUMesh>, Core::WeakPtr<Model>> mMeshToModels;
    Core::WeakPtr<GPUShader> mDefaultModelShader;
public:
    GET(DefaultModelShader)
    CRGET(Models)
    CRGET(MeshToModels)
};
REGISTER_CLASS(ModelManager);
