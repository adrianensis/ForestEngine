#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Buffers/GPUDefinitions.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Model.hpp"

void Material::init(const MaterialData& materialData, u32 id)
{
    mMaterialData = materialData;
	mID = id;

    FOR_RANGE(i, 0, mMaterialData.mTexturePaths.size())
    {
        mTextures[i] = GET_SYSTEM(MaterialManager).loadTexture(mMaterialData.mTexturePaths[i], mMaterialData.mCreateMipMap);
    }

    mUniforms.push_back(GPUBuiltIn::Uniforms::mProjectionMatrix);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mViewMatrix);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mIsInstanced);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mIsAnimated);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mUseVertexColor);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mUseColorAsTint);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mHasTexture);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mAlphaEnabled);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mHasBorder);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mTime);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mWindowSize);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mBonesTransform);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mBaseColor);
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
        mTextures[(u32)TextureType::BASE_COLOR]->bind();
	}

	Ptr<Camera> camera = GET_SYSTEM(RenderEngine).mCamera;

	const Matrix4& projectionMatrix = camera->mProjectionMatrix;
	const Matrix4& viewMatrix = camera->mViewMatrix;

	shader->addMatrix(isWorldSpace ? projectionMatrix : Matrix4::smIdentity, GPUBuiltIn::Uniforms::mProjectionMatrix.mName);
	shader->addMatrix(isWorldSpace ? viewMatrix : Matrix4::smIdentity, GPUBuiltIn::Uniforms::mViewMatrix.mName);

	shader->addBool(isInstanced, GPUBuiltIn::Uniforms::mIsInstanced.mName);

	shader->addBool(mTextures[(u32)TextureType::BASE_COLOR].isValid(), GPUBuiltIn::Uniforms::mHasTexture.mName);
	shader->addBool(mMaterialData.mAlphaEnabled, GPUBuiltIn::Uniforms::mAlphaEnabled.mName);
	shader->addBool(mMaterialData.mHasBorder, GPUBuiltIn::Uniforms::mHasBorder.mName);
    shader->addBool(mMaterialData.mUseVertexColor, GPUBuiltIn::Uniforms::mUseVertexColor.mName);
    shader->addBool(mMaterialData.mUseColorAsTint, GPUBuiltIn::Uniforms::mUseColorAsTint.mName);
	shader->addVector4(mMaterialData.mBaseColor, GPUBuiltIn::Uniforms::mBaseColor.mName);

	shader->addFloat(GET_SYSTEM(Time).getDeltaTimeSeconds(), GPUBuiltIn::Uniforms::mTime.mName);

	shader->addVector2(GET_SYSTEM(RenderContext).getWindowSize(), GPUBuiltIn::Uniforms::mWindowSize.mName);


    shader->addBool(isAnimated, GPUBuiltIn::Uniforms::mIsAnimated.mName);
    
    if(isAnimated)
    {
        const std::vector<Matrix4> & transforms = GET_SYSTEM(AnimationManager).getBoneTransforms(model);
        shader->addMatrixArray(transforms, GPUBuiltIn::Uniforms::mBonesTransform.mName);
    }
}

IMPLEMENT_SERIALIZATION(Material)
{
}

IMPLEMENT_DESERIALIZATION(Material)
{
}
