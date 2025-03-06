#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "GPU/GPUInstance.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/Shader/GPUShader.hpp"
#include "Graphics/Model/ModelManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "Core/EntityComponent/EntityPtr.hpp"

void RenderPass::init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData)
{
    mRenderPipeline = renderPipeline;
    mRenderPassData = renderPassData;

    mGPURenderPass = new GPURenderPass();
    if (!mGPURenderPass->init(GET_SYSTEM(GPUInstance).mGPUContext, renderPassData.mGPURenderPassData))
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

    mGPURenderPass->terminate();
    delete mGPURenderPass;
}

void RenderPass::addRenderer(TComponentPtr<MeshRenderer> renderer)
{
    PROFILER_CPU_NAMED(RenderPass_add_renderer)
	InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);
    mInstancedMeshRenderers.insert(instancedMeshData);
}

OwnerPtr<GPUShaderPipeline> RenderPass::compileShader(TComponentPtr<MeshRenderer> renderer)
{
    PROFILER_CPU_NAMED(RenderPass_add_renderer)
	InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

    std::vector<GPUUniformBuffer> uniformBuffers;
    uniformBuffers.push_back(GET_SYSTEM(GPUShaderManager).getGPUShaderPropertiesGPUUniformBuffer(instancedMeshData.mShader));

    WeakPtr<Model> model = GET_SYSTEM(ModelManager).getModelFromMesh(instancedMeshData.mMesh);
    if(model)
    {
        WeakPtr<GPUSkeletonState> skeletonState = model->getSkeletonState();
        if(skeletonState)
        {
            uniformBuffers.push_back(GET_SYSTEM(GPUSkeletalAnimationManager).getSkeletonRenderStateGPUUniformBuffer(skeletonState));
        }
    }

    uniformBuffers.push_back(mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData));
    uniformBuffers.push_back(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices));

    WeakPtr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
    GPUShaderPipelineDepthStencilData gpuGPUShaderPipelineDepthStencilData;
    gpuGPUShaderPipelineDepthStencilData.mDepthTestEnable = VK_TRUE; //bool
    gpuGPUShaderPipelineDepthStencilData.mDepthWriteEnable = VK_TRUE; //bool
    gpuGPUShaderPipelineDepthStencilData.mDepthCompareOp = VK_COMPARE_OP_LESS; //VkCompareOp
    gpuGPUShaderPipelineDepthStencilData.mDepthBoundsTestEnable = VK_FALSE; //bool
    gpuGPUShaderPipelineDepthStencilData.mStencilTestEnable = instancedMeshData.mGPUShaderStencilData.mUseStencil; //bool
    
    VkStencilOpState vkStencilOpState;
    vkStencilOpState.failOp = (VkStencilOp) instancedMeshData.mGPUShaderStencilData.mStencilFailOp;
    vkStencilOpState.passOp = (VkStencilOp) instancedMeshData.mGPUShaderStencilData.mStencilPassOp;
    vkStencilOpState.depthFailOp = (VkStencilOp) instancedMeshData.mGPUShaderStencilData.mDepthFailOp;
    vkStencilOpState.compareOp = (VkCompareOp) instancedMeshData.mGPUShaderStencilData.mStencilFunction;
    vkStencilOpState.compareMask = 0xFF;
    vkStencilOpState.writeMask = 0xFF;
    vkStencilOpState.reference = instancedMeshData.mGPUShaderStencilData.mStencilValue;

    gpuGPUShaderPipelineDepthStencilData.mStencilFront = vkStencilOpState;
    gpuGPUShaderPipelineDepthStencilData.mStencilBack = vkStencilOpState;
    gpuGPUShaderPipelineDepthStencilData.mMinDepthBounds = 0; //float
    gpuGPUShaderPipelineDepthStencilData.mMaxDepthBounds = 0; //float
    GPUShaderCompilationData shaderCompilationData
    {
        instancedMeshData.mMesh,
        mGPURenderPass,
        ClassManager::getDynamicClassMetadata(this).mClassDefinition.mName,
        HashedString(std::to_string(instancedMeshData.mShader->getID())),
        uniformBuffers,
        instancedMeshRenderer->getGPUVertexBuffersContainer(),
        gpuGPUShaderPipelineDepthStencilData
    };

    return instancedMeshData.mShader->compileShader(shaderCompilationData);
}

void RenderPass::removeRenderer(TComponentPtr<MeshRenderer> renderer)
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
    WeakPtr<GPUShaderPipeline> gpuGPUShaderPipeline = mRenderPipeline->getGPUShaderPipelines().at(instancedMeshData);
    gpuGPUShaderPipeline->enable();
    instancedMeshRenderer->render();
    gpuGPUShaderPipeline->disable();
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
        PROFILER_GPU_NAMED(renderPass, mGPURenderPass->mGPUContext->mTracyContext, mGPURenderPass->mGPUContext->vulkanCommandBuffers[mGPURenderPass->mGPUContext->currentFrame].getVkCommandBuffer())
        render();
    }
    mGPURenderPass->end();
}

void RenderPass::updateGlobalData()
{
	PROFILER_CPU()

    Matrix4 projectionViewMatrix = calculateProjectionViewMatrix();
    TComponentPtr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

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

    TComponentPtr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 projectionViewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mProjectionMatrix : ortho;
    Matrix4 viewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mViewMatrix : view2D;

    projectionViewMatrix.mul(viewMatrix);

    return projectionViewMatrix;
}

// void RenderPass::setupShader(WeakPtr<GPUShader> shader) const
// {
//     PROFILER_CPU()
//     FOR_ARRAY(i, mRenderPassData.mDependencies)
//     {
//         // FramebufferBinding framebufferBinding
//         // {
//             // mRenderPassData.mDependencies[i].mSamplerName,
//             // mRenderPassData.mDependencies[i].mRenderPass->getOutputGPUFramebuffer().getAttachments().at(mRenderPassData.mDependencies[i].mAttachmentType).mAttachmentID,
//             // mRenderPassData.mDependencies[i].mStage
//         // };
//         // shader->addFramebufferBinding(framebufferBinding);
//     }
// }

void RenderPass::onResize()
{
    mGPURenderPass->onResize();
}