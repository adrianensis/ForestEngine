#pragma once

// CPP_IGNORE

#include "Core/Module.hpp"

class Texture;
class Shader;

class Material: public ObjectBase
{
    GENERATE_METADATA(Material)
private: 
	Vector4 mColor;
	Shader* mShader = nullptr;
	Texture* mTexture = nullptr;
	bool mAlphaEnabled = false;
	bool mHasBorder = false;

public:
	Material();
	void init();
	void bind(bool isWorldSpace, bool isInstanced);
	void enable();
	void disable();

	void serialize(JSON& json) const override;
	void deserialize(const JSON& json) override;

	GET_SET(Color)
	GET_SET(Shader)
	GET_SET(Texture)
	GET_SET(AlphaEnabled)
	GET_SET(HasBorder)
};