#ifndef MATERIAL_HPP
#define MATERIAL_HPP

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

	virtual u64 getHash() const override;

private: 
	Vector4 mColor;
	Ptr<Shader> mShader;
	Ptr<Texture> mTexture;
	bool mAlphaEnabled = false;
	bool mHasBorder = false;

public:
	GET_SET(Color)
	RGET_SET(Shader)
	RGET_SET(Texture)
	GET_SET(AlphaEnabled)
	GET_SET(HasBorder)
};

#endif