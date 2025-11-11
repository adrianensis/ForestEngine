#pragma once

#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Scene/GameComponent.hpp"

class Model;

class ModelRendererData: public GPURenderItemData
{
public:
	Core::WeakPtr<const Model> mModel;
};

class ModelRenderer: public GameComponent
{
    
public:
    void init(const ModelRendererData& data);
    
    void onComponentAdded() override;
    void onDestroy() override;

private:
    ModelRendererData mModelRendererData;

public:
    CRGET(ModelRendererData)
};
REGISTER_CLASS(ModelRenderer);
