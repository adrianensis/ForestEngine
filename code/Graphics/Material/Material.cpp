#include "Graphics/Material/Material.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderEngine.hpp"


Material::Material()
{
}

void Material::init()
{
}

void Material::bind(bool isWorldSpace, bool isInstanced)
{
	PROFILER_CPU()

	if (mTexture)
	{
		mTexture.get().bind();
	}

	Ptr<Camera> camera = RenderEngine::getInstance().mCamera;

	const Matrix4& projectionMatrix = camera.get().mProjectionMatrix;
	const Matrix4& viewMatrix = camera.get().mViewMatrix;

	mShader.get().addMatrix(isWorldSpace ? projectionMatrix : Matrix4::getIdentity(), "projectionMatrix");
	mShader.get().addMatrix(isWorldSpace ? viewMatrix : Matrix4::getIdentity(), "viewMatrix");

	mShader.get().addBool(isInstanced, "isInstanced");

	mShader.get().addBool(mTexture.isValid(), "hasTexture");
	mShader.get().addBool(mAlphaEnabled, "alphaEnabled");
	mShader.get().addBool(mHasBorder, "hasBorder");

	mShader.get().addFloat(Time::getInstance().getDeltaTimeSeconds(), "time");

	mShader.get().addVector2(RenderContext::getWindowSize(), "windowSize");
}

void Material::enable()
{
	mShader.get().enable();
}

void Material::disable()
{
	mShader.get().disable();
}

IMPLEMENT_SERIALIZATION(Material)
{
}

IMPLEMENT_DESERIALIZATION(Material)
{
}
