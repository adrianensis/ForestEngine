#pragma once

#include "Core/Module.hpp"

class Texture;
class Shader;

class Material: public ObjectBase
{
    GENERATE_METADATA(Material)
	DECLARE_SERIALIZATION()

public:
    Material();
    void init();
    void bind(bool isWorldSpace, bool isInstanced) const;
    void enable() const;
    void disable() const;

public:
	Ptr<Shader> mShader;
	Ptr<const Texture> mTexture;
	bool mAlphaEnabled = true;
	bool mHasBorder = false;
};
