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
    void bind(bool isWorldSpace, bool isInstanced);
    void enable();
    void disable();

public:
	Vector4 mColor;
	Ptr<Shader> mShader;
	Ptr<Texture> mTexture;
	bool mAlphaEnabled = true;
	bool mHasBorder = false;
};
