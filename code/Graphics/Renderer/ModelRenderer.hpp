#ifndef MODELRENDERER_HPP
#define MODELRENDERER_HPP

#include "Scene/Component.hpp"

#include "Graphics/Renderer/Renderer.hpp"

class Model;

class ModelRenderer: public Component
{
    GENERATE_METADATA(ModelRenderer)
	DECLARE_SERIALIZATION()
	
public:
    void init() override;
    void onComponentAdded() override;
    void onDestroy() override;

private:
	Ptr<const Model> mModel;
    bool mIsInstanced = false;

public:
	GET_SET(Model)
	GET_SET(IsInstanced)
};

#endif