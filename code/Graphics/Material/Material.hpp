#pragma once

#include "Core/Module.hpp"

class Texture;
class Shader;

class Material: public ObjectBase
{
    GENERATE_METADATA(Material)
	PRI Vector4 mColor;
	PRI Shader* mShader = nullptr;
	PRI Texture* mTexture = nullptr;
	PRI bool mAlphaEnabled = false;
	PRI bool mHasBorder = false;

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