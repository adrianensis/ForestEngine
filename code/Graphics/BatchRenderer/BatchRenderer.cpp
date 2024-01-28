#include "Graphics/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/GPU/GPUShader.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Light/Light.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Shader/ShaderUtils.hpp"

void BatchRenderer::init(const BatchData& batchData)
{
	mBatchData = batchData;
	mMeshBatcher.init(mBatchData);

    const GPUBuffersLayout& gpuBuffersLayout = mMeshBatcher.getGPUBuffersLayout();
    
    mShader = ShaderUtils::createShader(gpuBuffersLayout, mBatchData.mMaterial);
    
    bindSharedBuffers();
}

void BatchRenderer::terminate()
{
    mMeshBatcher.terminate();
}

void BatchRenderer::bindSharedBuffers()
{
    mShader->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).mGlobalDataBuffer);

    mShader->bindSharedBuffer(mMeshBatcher.getGPUBuffersLayout().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices));

    if(mBatchData.mMaterial->getMaterialData().mReceiveLight)
    {
        mShader->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).mLightDataBuffer);
    }

    if(mBatchData.mMaterial->getMaterialData().mIsSkinned)
    {
        mShader->bindSharedBuffer(mMeshBatcher.getGPUBuffersLayout().getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices));
    }
    
    mShader->bindSharedBuffer(mMeshBatcher.getGPUBuffersLayout().getSharedBuffer(mBatchData.mMaterial->getInstancedPropertiesSharedBufferData()));
}

void BatchRenderer::render()
{
	PROFILER_CPU()

	if (!mRenderers.empty())
	{
        enable();

		if(shouldRegenerateBuffers())
		{
			updateBuffers();
		}

        updateMaterialProperties();

        mMeshBatcher.drawCall();

        disable();
	}
}

void BatchRenderer::enable()
{
    mShader->enable();
    mMeshBatcher.enable();
    mBatchData.mMaterial->enable();

    if(mBatchData.mMaterial->getMaterialData().mIsSkinned)
    {
        mMeshBatcher.updateBoneTransforms();
    }

    if(mBatchData.mStencilData.mUseStencil)
    {
        GET_SYSTEM(GPUInterface).enableStencil(mBatchData.mStencilData.mStencilValue, mBatchData.mStencilData.mStencilFunction, mBatchData.mStencilData.mStencilPassOp);
    }
}

void BatchRenderer::disable()
{
    if(mBatchData.mStencilData.mUseStencil)
    {
        GET_SYSTEM(GPUInterface).disableStencil();
    }

    mBatchData.mMaterial->disable();
    mMeshBatcher.disable();
    mShader->disable();
}

void BatchRenderer::addRenderer(Ptr<MeshRenderer> renderer)
{
	mRenderers.push_back(renderer);
	renderer->setBatchRenderer(getPtrToThis());
	mRegenerateBuffersRequested = true;
}

void BatchRenderer::updateBuffers()
{
	PROFILER_CPU()
    
    std::vector<Ptr<MeshRenderer>> newList;
    FOR_ARRAY(i, mRenderers)
    {
        PROFILER_BLOCK_CPU("remove");

        Ptr<MeshRenderer> renderer = mRenderers[i];
        if(renderer.isValid())
        {
            if (renderer->getIsPendingToBeDestroyed())
            {
                renderer->finallyDestroy();
            }
            else
            {
                newList.push_back(renderer);
            }
        }
    }

    mRenderers.clear();
    mRenderers = newList;

    u32 newSize = mRenderers.size();
    mMeshBatcher.resize(newSize);

    FOR_ARRAY(i, mRenderers)
    {
        Ptr<MeshRenderer> renderer = mRenderers[i];

        //Ptr<Camera> camera = GET_SYSTEM(RenderEngine).mCamera;
        //if(camera && camera->mFrustum.testSphere(renderer->mGameObject->mTransform->getWorldPosition(), renderer->mGameObject->mTransform->getLocalScale().x))
        {
            PROFILER_BLOCK_CPU("update");
            renderer->update();
            const Matrix4& rendererModelMatrix = renderer->getRendererModelMatrix();
            mMeshBatcher.addInstance(rendererModelMatrix, renderer->getMeshInstance());
        }
    }

    mRegenerateBuffersRequested = false;
}

void BatchRenderer::updateMaterialProperties()
{
	PROFILER_CPU()
    FOR_ARRAY(i, mRenderers)
    {
        Ptr<MeshRenderer> renderer = mRenderers[i];
        mMeshBatcher.setMaterialInstanceProperties(i, renderer->getMaterialInstance().mMaterialInstancedProperties);
    }
}

bool BatchRenderer::shouldRegenerateBuffers() const
{
    // TODO: possible optimization for dynamic objects: only regenerate buffers when transform changes.
	return mRegenerateBuffersRequested || !mBatchData.mIsStatic;
}
