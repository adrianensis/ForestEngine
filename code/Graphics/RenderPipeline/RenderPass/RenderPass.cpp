#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"

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
      
        GPUVertexBuffersContainer gpuVertexBuffersContainer;
        batchData.mMesh->populateGPUVertexBuffersContainer(gpuVertexBuffersContainer, batchData.mIsStatic);

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
    
    Ptr<Model> model = GET_SYSTEM(ModelManager).getModelFromMesh(batchData.mMesh);
    if(model)
    {
        Ptr<GPUSkeletonState> skeletonState = model->getSkeletonState();
        if(skeletonState)
        {
            mGPUPrograms.at(batchData)->bindSharedBuffer(GET_SYSTEM(GPUSkeletalAnimationManager).getSkeletonRenderStateGPUSharedBuffer(skeletonState));
        }
    }

    mGPUPrograms.at(batchData)->bindSharedBuffer(GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData));
    mGPUPrograms.at(batchData)->bindSharedBuffer(GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices));

    Ptr<Shader> shader = getShader(batchData);
    shader->bindTextures(mGPUPrograms.at(batchData), GET_SYSTEM(MaterialManager).getMaterialTextureBindings(batchData.mMaterial));
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

void RenderPass::renderBatch(const BatchData& batchData)
{
    Ptr<BatchRenderer> batchRenderer = mRenderPipeline->getBatchMap().at(batchData);
    Ptr<Shader> shader = getShader(batchData);
    mGPUPrograms.at(batchData)->enable();
    shader->enable();
    batchRenderer->render();
    shader->disable();
    mGPUPrograms.at(batchData)->disable();
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