#pragma once

#include "Engine/EntityComponent/Component.hpp"

#include "Graphics/MeshRenderer/MeshRenderer.hpp"

class Model;

class ModelRendererData: public GPURenderItemData
{
public:
	Core::WeakPtr<const Model> mModel;
};

class ModelRenderer: public EC::Component
{
    
	DECLARE_SERIALIZATION()
	
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
