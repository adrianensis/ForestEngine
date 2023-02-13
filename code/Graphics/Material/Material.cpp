#include "Graphics/Material/Material.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderEngine.hpp"


Material::Material()
{
}

void Material::init(u32 id)
{
	mID = id;
}

void Material::bind(bool isWorldSpace, bool isInstanced) const
{
	PROFILER_CPU()

	if (mTextures[(u32)TextureType::DIFFUSE])
	{
        mTextures[(u32)TextureType::DIFFUSE].get().bind();
	}

	Ptr<Camera> camera = GET_SYSTEM(RenderEngine).mCamera;

	const Matrix4& projectionMatrix = camera.get().mProjectionMatrix;
	const Matrix4& viewMatrix = camera.get().mViewMatrix;

	mShader.get().addMatrix(isWorldSpace ? projectionMatrix : Matrix4::getIdentity(), "projectionMatrix");
	mShader.get().addMatrix(isWorldSpace ? viewMatrix : Matrix4::getIdentity(), "viewMatrix");

	mShader.get().addBool(isInstanced, "isInstanced");

	mShader.get().addBool(mTextures[(u32)TextureType::DIFFUSE].isValid(), "hasTexture");
	mShader.get().addBool(mAlphaEnabled, "alphaEnabled");
	mShader.get().addBool(mHasBorder, "hasBorder");

	mShader.get().addFloat(GET_SYSTEM(Time).getDeltaTimeSeconds(), "time");

	mShader.get().addVector2(RenderContext::getWindowSize(), "windowSize");
}

void Material::enable() const
{
	mShader.get().enable();
}

void Material::disable() const
{
	mShader.get().disable();
}

IMPLEMENT_SERIALIZATION(Material)
{
}

IMPLEMENT_DESERIALIZATION(Material)
{
}
