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

void Material::bind(Ptr<Shader> shader, bool isWorldSpace, bool isInstanced, bool isAnimated, Ptr<const Model> model) const
{
	PROFILER_CPU()

	if (mTextures[(u32)TextureType::BASE_COLOR])
	{
        mTextures[(u32)TextureType::BASE_COLOR].get().bind();
	}

	Ptr<Camera> camera = GET_SYSTEM(RenderEngine).mCamera;

	const Matrix4& projectionMatrix = camera.get().mProjectionMatrix;
	const Matrix4& viewMatrix = camera.get().mViewMatrix;

	shader.get().addMatrix(isWorldSpace ? projectionMatrix : Matrix4::smIdentity, GPUBuiltIn::Uniforms::mProjectionMatrix.mName);
	shader.get().addMatrix(isWorldSpace ? viewMatrix : Matrix4::smIdentity, GPUBuiltIn::Uniforms::mViewMatrix.mName);

	shader.get().addBool(isInstanced, GPUBuiltIn::Uniforms::mIsInstanced.mName);

	shader.get().addBool(mTextures[(u32)TextureType::BASE_COLOR].isValid(), GPUBuiltIn::Uniforms::mHasTexture.mName);
	shader.get().addBool(mAlphaEnabled, GPUBuiltIn::Uniforms::mAlphaEnabled.mName);
	shader.get().addBool(mHasBorder, GPUBuiltIn::Uniforms::mHasBorder.mName);

	shader.get().addFloat(GET_SYSTEM(Time).getDeltaTimeSeconds(), GPUBuiltIn::Uniforms::mTime.mName);

	shader.get().addVector2(GET_SYSTEM(RenderContext).getWindowSize(), GPUBuiltIn::Uniforms::mWindowSize.mName);

    shader.get().addBool(isAnimated, GPUBuiltIn::Uniforms::mIsAnimated.mName);
    
    if(isAnimated)
    {
        const std::vector<Matrix4> & transforms = GET_SYSTEM(AnimationManager).getBoneTransforms(model);
        shader.get().addMatrixArray(transforms, GPUBuiltIn::Uniforms::mBonesTransform.mName);
    }
}

IMPLEMENT_SERIALIZATION(Material)
{
}

IMPLEMENT_DESERIALIZATION(Material)
{
}
