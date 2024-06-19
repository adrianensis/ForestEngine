#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Model/SkeletalAnimation/SkeletalAnimationManager.hpp"

void RenderPass::init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData)
{
    mRenderPipeline = renderPipeline;
    mRenderPassData = renderPassData;

    if(mRenderPassData.mOutputFramebufferData.isValid())
    { 
        mOutputGPUFramebuffer.init(mRenderPassData.mOutputFramebufferData);
    }
}

void RenderPass::terminate()
{
}

void RenderPass::addRenderer(Ptr<MeshRenderer> renderer)
{
	BatchData batchData;
	batchData.init(renderer);

	if (!mBatches.contains(batchData))
	{
        mBatches.insert(batchData);
        mGPUPrograms.emplace(batchData, OwnerPtr<GPUProgram>());
      
        bool isStatic = batchData.mIsStatic || batchData.mIsInstanced;
        GPUVertexBuffersContainer gpuVertexBuffersContainer;
        batchData.mMesh->populateGPUVertexBuffersContainer(gpuVertexBuffersContainer, isStatic, batchData.mIsInstanced);

        Ptr<Shader> shader = getShader(batchData);
        setupShader(shader);
        mGPUPrograms.insert_or_assign(batchData, ShaderUtils::createShaderCustomFragment(
            ClassManager::getDynamicClassMetadata(this).mClassDefinition.mName,
            gpuVertexBuffersContainer,
            batchData.mMaterial.get(),
            shader
        ));

        bindShader(batchData);
    }
}

void RenderPass::removeRenderer(Ptr<MeshRenderer> renderer)
{
    BatchData batchData;
	batchData.init(renderer);

    Ptr<BatchRenderer> batchRenderer = mRenderPipeline->getBatchMap().at(batchData);
    if(batchRenderer->isEmpty())
    {
        mBatches.erase(batchData);
    }
}

void RenderPass::preFramebufferEnabled()
{
}

void RenderPass::postFramebufferEnabled()
{
}

void RenderPass::bindShader(const BatchData& batchData)
{
    mGPUPrograms.at(batchData)->bindSharedBuffer(GET_SYSTEM(MaterialManager).getMaterialPropertiesGPUSharedBuffer(batchData.mMaterial));
    if(GET_SYSTEM(SkeletalAnimationManager).getSkeletonStates().contains(batchData.mMesh->mModel))
    {
        Ptr<const SkeletonState> skeletonState = GET_SYSTEM(SkeletalAnimationManager).getSkeletonStates().at(batchData.mMesh->mModel);
        mGPUPrograms.at(batchData)->bindSharedBuffer(GET_SYSTEM(SkeletalAnimationManager).getSkeletonRenderStateGPUSharedBuffer(skeletonState));
    }

    mGPUPrograms.at(batchData)->bindSharedBuffer(GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData));
    mGPUPrograms.at(batchData)->bindSharedBuffer(GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices));

    if(batchData.mMaterial->getMaterialData().mReceiveLight)
    {
        mGPUPrograms.at(batchData)->bindSharedBuffer(GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mLightsBufferData));
        mGPUPrograms.at(batchData)->bindSharedBuffer(GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mShadowMappingBufferData));
    }

    Ptr<Shader> shader = getShader(batchData);
    shader->bindTextures(mGPUPrograms.at(batchData));
}

void RenderPass::preRender()
{
}

void RenderPass::postRender()
{
}

void RenderPass::render()
{
}

void RenderPass::renderPass()
{
	PROFILER_CPU()

    updateGlobalData();

    if(mRenderPassData.mOutputFramebufferData.isValid())
    {
        preFramebufferEnabled();
        mOutputGPUFramebuffer.enable(GPUFramebufferOperationType::READ_AND_DRAW);
        postFramebufferEnabled();
    }

    preRender();
    render();
    postRender();

    if(mRenderPassData.mOutputFramebufferData.isValid())
    {
        mOutputGPUFramebuffer.disable(GPUFramebufferOperationType::READ_AND_DRAW);
    }
}

void RenderPass::updateGlobalData()
{
	PROFILER_CPU()

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);

    Ptr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 projectionViewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mProjectionMatrix : ortho;
    Matrix4 viewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mViewMatrix : Matrix4::smIdentity;

    projectionViewMatrix.mul(viewMatrix);

    GPUBuiltIn::SharedBuffers::GPUGlobalData gpuGlobalData =
    {
        projectionViewMatrix,
        camera->mGameObject->mTransform->getWorldPosition()
    };
	GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).setData(gpuGlobalData);
}

Ptr<Shader> RenderPass::getShader(const BatchData& batchData) const
{
    Ptr<Shader> shader = batchData.mMaterial->getShader();
    return shader;
}

void RenderPass::setupShader(Ptr<Shader> shader) const
{
    FOR_ARRAY(i, mRenderPassData.mDependencies)
    {
        shader->addFramebufferBinding(mRenderPassData.mDependencies[i]);
    }
}