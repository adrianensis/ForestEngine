#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Material/Material.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderEngine.hpp"
#endif

class Texture;
class Shader;

class Material: public ObjectBase
{
    GENERATE_METADATA(Material)
public:
	CPP Material()
	{
		mAlphaEnabled = true;
		mHasBorder = false;
	}

	CPP void init()
	{
	}

	CPP void bind(bool isWorldSpace, bool isInstanced)
	{
		if (mTexture)
		{
			mTexture.get().bind();
		}

		Ptr<Camera> camera = RenderEngine::getInstance().getCamera();

		const Matrix4& projectionMatrix = camera.get().getProjectionMatrix();
		const Matrix4& viewMatrix = camera.get().getViewMatrix();

		mShader.get().addMatrix(isWorldSpace ? projectionMatrix : Matrix4::getIdentity(), "projectionMatrix");
		mShader.get().addMatrix(isWorldSpace ? viewMatrix : Matrix4::getIdentity(), "viewMatrix");

		mShader.get().addBool(isInstanced, "isInstanced");

		mShader.get().addBool(mTexture.isValid(), "hasTexture");
		mShader.get().addBool(mAlphaEnabled, "alphaEnabled");
		mShader.get().addBool(mHasBorder, "hasBorder");

		mShader.get().addFloat(Time::getInstance().getDeltaTimeSeconds(), "time");

		mShader.get().addVector2(RenderContext::getWindowSize(), "windowSize");
	}

	CPP void enable()
	{
		mShader.get().enable();
	}

	CPP void disable()
	{
		mShader.get().disable();
	}

	CPP void serialize(JSON& json) const
	{
		//DO_SERIALIZE("texture", mTexture)
	}

	CPP void deserialize(const JSON& json)
	{
		//DO_DESERIALIZE("texture", mTexture)
	}

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