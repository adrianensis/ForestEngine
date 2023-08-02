#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Model.hpp"

void Material::init(const MaterialData& materialData, u32 id)
{
    mMaterialData = materialData;
	mID = id;

    FOR_RANGE(i, 0, mMaterialData.mTexturePaths.size())
    {
        if(!mMaterialData.mTexturePaths[i].empty())
        {
            mTextures[i] = GET_SYSTEM(MaterialManager).loadTexture(mMaterialData.mTexturePaths[i], mMaterialData.mCreateMipMap);
        }
    }

    mUniforms.push_back(GPUBuiltIn::Uniforms::mProjectionMatrix);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mViewMatrix);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mTime);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mWindowSize);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mBaseColor);
    mUniforms.push_back(GPUBuiltIn::Uniforms::mSampler);

    if(materialData.mIsSkinned)
    {
        mUniforms.push_back(GPUBuiltIn::Uniforms::mBonesTransform);

        mConsts.push_back(GPUBuiltIn::Consts::MAX_BONES);
        mConsts.push_back(GPUBuiltIn::Consts::MAX_BONE_INFLUENCE);
    }

    mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mTextureCoord);
    if(mMaterialData.mUseVertexColor)
    {
        mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mColor);
    }
    
    mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mTextureCoord);
    mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mColor);

    mFragmentOutputs.push_back(GPUBuiltIn::FragmentOutput::mColor);
}

void Material::bind(Ptr<Shader> shader, bool isWorldSpace, bool isInstanced, Ptr<const Mesh> mesh) const
{
	PROFILER_CPU()

	if (mTextures[(u32)TextureType::BASE_COLOR])
	{
        mTextures[(u32)TextureType::BASE_COLOR]->bind();
	}

	Ptr<Camera> camera = GET_SYSTEM(RenderEngine).mCamera;

	const Matrix4& projectionMatrix = camera->mProjectionMatrix;
	const Matrix4& viewMatrix = camera->mViewMatrix;

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);

	shader->addMatrix(isWorldSpace ? projectionMatrix : ortho, GPUBuiltIn::Uniforms::mProjectionMatrix.mName);
	shader->addMatrix(isWorldSpace ? viewMatrix : Matrix4::smIdentity, GPUBuiltIn::Uniforms::mViewMatrix.mName);

	shader->addVector4(mMaterialData.mBaseColor, GPUBuiltIn::Uniforms::mBaseColor.mName);

	shader->addFloat(GET_SYSTEM(Time).getDeltaTimeSeconds(), GPUBuiltIn::Uniforms::mTime.mName);

	shader->addVector2(GET_SYSTEM(RenderContext).getWindowSize(), GPUBuiltIn::Uniforms::mWindowSize.mName);

	if(mesh->mModel)
	{
		bool isAnimated = mesh->mModel->isAnimated();
        if(isAnimated)
        {
            const std::vector<Matrix4> & transforms = GET_SYSTEM(AnimationManager).getBoneTransforms(mesh->mModel);
            shader->addMatrixArray(transforms, GPUBuiltIn::Uniforms::mBonesTransform.mName);
        }
	}
}

bool Material::hasTexture() const
{
    return mTextures[(u32)TextureType::BASE_COLOR].isValid();
}

IMPLEMENT_SERIALIZATION(Material)
{
}

IMPLEMENT_DESERIALIZATION(Material)
{
}
