#pragma once

#include "Core/Module.hpp"

class Texture;
class Shader;

class Material: public ObjectBase
{
    GENERATE_METADATA(Material)
	PRI Vector4 mColor; GET_SET(Color)
	PRI Shader* mShader = nullptr; GET_SET(Shader);
	PRI Texture* mTexture = nullptr; GET_SET(Texture);
	PRI bool mAlphaEnabled = false; GET_SET(AlphaEnabled)
	PRI bool mHasBorder = false; GET_SET(HasBorder)

PUB
	Material();
	void init();
	void bind(bool isWorldSpace, bool isInstanced);
	void enable();
	void disable();

	void serialize(JSON& json) const override;
	void deserialize(const JSON& json) override;
};