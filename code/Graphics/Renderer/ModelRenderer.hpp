#pragma once

#include "Core/ECS/Component.hpp"

#include "Graphics/Renderer/MeshRenderer.hpp"

class Model;

class ModelRendererData: public RendererData
{
public:
	Ptr<const Model> mModel;
};

class ModelRenderer: public Component
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
