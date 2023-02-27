#include "Graphics/Material/Material.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Buffers/GPUDefinitions.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Model.hpp"

Material::Material()
{
}

void Material::init(u32 id)
{
	mID = id;

    mUniforms.push_back(GPUBuiltIn::Uniforms::mProjectionMatrix);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mViewMatrix);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mIsInstanced);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mIsAnimated);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mHasTexture);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mAlphaEnabled);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mHasBorder);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mTime);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mWindowSize);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mBonesTransform);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mSampler);

    mConsts.push_back(GPUBuiltIn::Consts::MAX_BONES);
    mConsts.push_back(GPUBuiltIn::Consts::MAX_BONE_INFLUENCE);

    mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mTexture);
    mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mColor);

    mFragmentOutputs.push_back(GPUBuiltIn::FragmentOutput::mColor);
}

void Material::bind(bool isWorldSpace, bool isInstanced, bool isAnimated, Ptr<const Model> model) const
{
	PROFILER_CPU()

	if (mTextures[(u32)TextureType::DIFFUSE])
	{
        mTextures[(u32)TextureType::DIFFUSE].get().bind();
	}

	Ptr<Camera> camera = GET_SYSTEM(RenderEngine).mCamera;

	const Matrix4& projectionMatrix = camera.get().mProjectionMatrix;
	const Matrix4& viewMatrix = camera.get().mViewMatrix;

	mShader.get().addMatrix(isWorldSpace ? projectionMatrix : Matrix4::getIdentity(), GPUBuiltIn::Uniforms::mProjectionMatrix.mName);
	mShader.get().addMatrix(isWorldSpace ? viewMatrix : Matrix4::getIdentity(), GPUBuiltIn::Uniforms::mViewMatrix.mName);

	mShader.get().addBool(isInstanced, GPUBuiltIn::Uniforms::mIsInstanced.mName);

	mShader.get().addBool(mTextures[(u32)TextureType::DIFFUSE].isValid(), GPUBuiltIn::Uniforms::mHasTexture.mName);
	mShader.get().addBool(mAlphaEnabled, GPUBuiltIn::Uniforms::mAlphaEnabled.mName);
	mShader.get().addBool(mHasBorder, GPUBuiltIn::Uniforms::mHasBorder.mName);

	mShader.get().addFloat(GET_SYSTEM(Time).getDeltaTimeSeconds(), GPUBuiltIn::Uniforms::mTime.mName);

	mShader.get().addVector2(GET_SYSTEM(RenderContext).getWindowSize(), GPUBuiltIn::Uniforms::mWindowSize.mName);

    mShader.get().addBool(isAnimated, GPUBuiltIn::Uniforms::mIsAnimated.mName);
    
    if(isAnimated)
    {
        const std::vector<Matrix4> & transforms = AnimationManager::getInstance().getBoneTransforms(model);
        mShader.get().addMatrixArray(transforms, GPUBuiltIn::Uniforms::mBonesTransform.mName);
    }
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
