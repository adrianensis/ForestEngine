#pragma once

#include "Scene/Component.hpp"

#include "Graphics/Renderer/Renderer.hpp"

class Model;

class ModelRendererData: public RendererData
{
public:
	Ptr<const Model> mModel;
};

class ModelRenderer: public ComponentWithData<ModelRendererData>
{
    GENERATE_METADATA(ModelRenderer)
	DECLARE_SERIALIZATION()
	
public:
    void init(ModelRendererData& data) override;
    
    void onComponentAdded() override;
    void onDestroy() override;
};
