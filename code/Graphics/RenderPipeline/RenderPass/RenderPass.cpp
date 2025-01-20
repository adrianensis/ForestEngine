#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "GPU/GPUInstance.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Model/ModelManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "Core/ECS/EntityHandler.hpp"

void RenderPass::init(WeakPtr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData)
{
    mRenderPipeline = renderPipeline;
    mRenderPassData = renderPassData;

    mGPURenderPass = new GPURenderPass(GET_SYSTEM(GPUInstance).mGPUContext);
    if (!mGPURenderPass->init(renderPassData.mGPURenderPassData))
    {
        CHECK_MSG(false, "Could not initialize render pass");
    }
    if(mRenderPassData.mOutputFramebufferData.isValid())
    { 
        // mOutputGPUFramebuffer.init(mRenderPassData.mOutputFramebufferData);
    }

    mGPUUniformBuffersContainer.addUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData, sizeof(GPUShaderDefinitions::UniformBuffers::GPUGlobalData), false);
}

void RenderPass::terminate()
{
    mGPUUniformBuffersContainer.terminate();

    FOR_MAP(it, mGPUShaderPipelines)
    {
        it->second->terminate();
    }

    mGPURenderPass->terminate();
    delete mGPURenderPass;
}

void RenderPass::addRenderer(TComponentHandler<MeshRenderer> renderer)
{
    PROFILER_CPU_NAMED(RenderPass_add_renderer)
	InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

    bool newInstancedMeshRenderer = !mInstancedMeshRenderers.contains(instancedMeshData);
	if (newInstancedMeshRenderer)
	{
        mInstancedMeshRenderers.insert(instancedMeshData);
        // compileShader(renderer);
    }
}

void RenderPass::compileShader(TComponentHandler<MeshRenderer> renderer)
{
    PROFILER_CPU_NAMED(RenderPass_add_renderer)
	InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

    if(mGPUShaderPipelines.contains(instancedMeshData))
    {
        mGPUShaderPipelines.at(instancedMeshData)->terminate();
        mGPUShaderPipelines.at(instancedMeshData).invalidate();
    }

    std::vector<GPUUniformBuffer> uniformBuffers;
    uniformBuffers.push_back(GET_SYSTEM(ShaderManager).getShaderPropertiesGPUUniformBuffer(instancedMeshData.mShader));

    WeakPtr<Model> model = GET_SYSTEM(ModelManager).getModelFromMesh(instancedMeshData.mMesh);
    if(model)
    {
        WeakPtr<GPUSkeletonState> skeletonState = model->getSkeletonState();
        if(skeletonState)
        {
            // uniformBuffers.push_back(GET_SYSTEM(GPUSkeletalAnimationManager).getSkeletonRenderStateGPUUniformBuffer(skeletonState));
        }
    }

    uniformBuffers.push_back(mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData));
    uniformBuffers.push_back(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices));

    WeakPtr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
    GPUShaderPipelineDepthStencilData gpuShaderPipelineDepthStencilData;
    gpuShaderPipelineDepthStencilData.mDepthTestEnable = VK_TRUE; //bool
    gpuShaderPipelineDepthStencilData.mDepthWriteEnable = VK_TRUE; //bool
    gpuShaderPipelineDepthStencilData.mDepthCompareOp = VK_COMPARE_OP_LESS; //VkCompareOp
    gpuShaderPipelineDepthStencilData.mDepthBoundsTestEnable = VK_FALSE; //bool
    gpuShaderPipelineDepthStencilData.mStencilTestEnable = instancedMeshData.mShaderStencilData.mUseStencil; //bool
    
    VkStencilOpState vkStencilOpState;
    vkStencilOpState.failOp = (VkStencilOp) instancedMeshData.mShaderStencilData.mStencilFailOp;
    vkStencilOpState.passOp = (VkStencilOp) instancedMeshData.mShaderStencilData.mStencilPassOp;
    vkStencilOpState.depthFailOp = (VkStencilOp) instancedMeshData.mShaderStencilData.mDepthFailOp;
    vkStencilOpState.compareOp = (VkCompareOp) instancedMeshData.mShaderStencilData.mStencilFunction;
    vkStencilOpState.compareMask = 0xFF;
    vkStencilOpState.writeMask = 0xFF;
    vkStencilOpState.reference = instancedMeshData.mShaderStencilData.mStencilValue;

    gpuShaderPipelineDepthStencilData.mStencilFront = vkStencilOpState;
    gpuShaderPipelineDepthStencilData.mStencilBack = vkStencilOpState;
    gpuShaderPipelineDepthStencilData.mMinDepthBounds = 0; //float
    gpuShaderPipelineDepthStencilData.mMaxDepthBounds = 0; //float
    ShaderCompilationData shaderCompilationData
    {
        instancedMeshData.mMesh,
        mGPURenderPass,
        ClassManager::getDynamicClassMetadata(this).mClassDefinition.mName,
        HashedString(std::to_string(renderer->getShaderInstance()->mShader->getID())),
        uniformBuffers,
        instancedMeshRenderer->getGPUVertexBuffersContainer(),
        gpuShaderPipelineDepthStencilData
    };

    mGPUShaderPipelines.emplace(instancedMeshData, instancedMeshData.mShader->compileShader(shaderCompilationData));
}

void RenderPass::removeRenderer(TComponentHandler<MeshRenderer> renderer)
{
    InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

    // WeakPtr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
    // if(instancedMeshRenderer->isEmpty())
    // {
    //     mInstancedMeshRenderers.erase(instancedMeshData);
    // }
}

void RenderPass::preFramebufferEnabled()
{
}

void RenderPass::postFramebufferEnabled()
{
}

void RenderPass::bindShader(const InstancedMeshData& instancedMeshData)
{
    PROFILER_CPU()
    WeakPtr<GPUShaderPipeline> gpuShaderPipeline = mGPUShaderPipelines.at(instancedMeshData);
    // gpuShaderPipeline->bindUniformBuffer(GET_SYSTEM(ShaderManager).getShaderPropertiesGPUUniformBuffer(instancedMeshData.mShader));
    
    // WeakPtr<Model> model = GET_SYSTEM(ModelManager).getModelFromMesh(instancedMeshData.mMesh);
    // if(model)
    // {
    //     WeakPtr<GPUSkeletonState> skeletonState = model->getSkeletonState();
    //     if(skeletonState)
    //     {
    //         gpuShaderPipeline->bindUniformBuffer(GET_SYSTEM(GPUSkeletalAnimationManager).getSkeletonRenderStateGPUUniformBuffer(skeletonState));
    //     }
    // }

    // gpuShaderPipeline->bindUniformBuffer(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData));
    // gpuShaderPipeline->bindUniformBuffer(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices));

    // mGPUShaderPipelines.at(instancedMeshData)->bindTextures(mGPUShaderPipelines.at(instancedMeshData)->getGPUShader(), GET_SYSTEM(ShaderManager).getShaderTextureBindings(instancedMeshData.mShader));
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

void RenderPass::renderInstancedMesh(const InstancedMeshData& instancedMeshData)
{
    PROFILER_CPU()
    WeakPtr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
    WeakPtr<GPUShaderPipeline> gpuShaderPipeline = mGPUShaderPipelines.at(instancedMeshData);

    gpuShaderPipeline->enable();
    instancedMeshRenderer->render();
    gpuShaderPipeline->disable();
}

void RenderPass::renderPass()
{
	PROFILER_CPU()

    updateGlobalData();

    // if(mRenderPassData.mOutputFramebufferData.isValid())
    // {
    //     preFramebufferEnabled();
    //     mOutputGPUFramebuffer.enable(GPUFramebufferOperationType::READ_AND_DRAW);
    //     postFramebufferEnabled();
    // }

    // preRender();
    // render();
    // postRender();

    // if(mRenderPassData.mOutputFramebufferData.isValid())
    // {
    //     mOutputGPUFramebuffer.disable(GPUFramebufferOperationType::READ_AND_DRAW);
    // }
    mGPURenderPass->begin();
    {
        PROFILER_GPU_NAMED(renderPass, mGPURenderPass->mGPUContext->mTracyContext, mGPURenderPass->mGPUContext->vulkanCommandBuffers[mGPURenderPass->mGPUContext->currentFrame]->getVkCommandBuffer())
        render();
    }
    mGPURenderPass->end();
}

void RenderPass::updateGlobalData()
{
	PROFILER_CPU()

    Matrix4 projectionViewMatrix = calculateProjectionViewMatrix();
    TComponentHandler<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    GPUShaderDefinitions::UniformBuffers::GPUGlobalData gpuGlobalData =
    {
        projectionViewMatrix,
        camera->getOwnerEntity()->getFirstComponent<Transform>()->getWorldPosition()
    };
	mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData).setData(gpuGlobalData);
}

Matrix4 RenderPass::calculateProjectionViewMatrix() const
{
	PROFILER_CPU()

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);
    Matrix4 view2D;
    view2D.view(Vector3(0,0,1000), Vector3(0,0,0));

    TComponentHandler<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 projectionViewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mProjectionMatrix : ortho;
    Matrix4 viewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mViewMatrix : view2D;

    projectionViewMatrix.mul(viewMatrix);

    return projectionViewMatrix;
}

void RenderPass::setupShader(WeakPtr<Shader> shader) const
{
    PROFILER_CPU()
    FOR_ARRAY(i, mRenderPassData.mDependencies)
    {
        // FramebufferBinding framebufferBinding
        // {
            // mRenderPassData.mDependencies[i].mSamplerName,
            // mRenderPassData.mDependencies[i].mRenderPass->getOutputGPUFramebuffer().getAttachments().at(mRenderPassData.mDependencies[i].mAttachmentType).mAttachmentID,
            // mRenderPassData.mDependencies[i].mStage
        // };
        // shader->addFramebufferBinding(framebufferBinding);
    }
}

void RenderPass::compile()
{
    
}