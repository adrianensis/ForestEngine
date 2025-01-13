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

    if(mRenderPassData.mOutputFramebufferData.isValid())
    { 
        // mOutputGPUFramebuffer.init(mRenderPassData.mOutputFramebufferData);
    }

    mGPUUniformBuffersContainer.addUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData, sizeof(GPUShaderDefinitions::UniformBuffers::GPUGlobalData), false);
}

void RenderPass::terminate()
{
    mGPUUniformBuffersContainer.terminate();

    FOR_MAP(it, mGPUShaders)
    {
        it->second->terminate();
    }
}

void RenderPass::addRenderer(TComponentHandler<MeshRenderer> renderer)
{
    PROFILER_CPU_NAMED(RenderPass_add_renderer)
	InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

	if (!mInstancedMeshRenderers.contains(instancedMeshData))
	{
        mInstancedMeshRenderers.insert(instancedMeshData);

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
        ShaderCompilationData shaderCompilationData
        {
            instancedMeshData.mMesh,
            mRenderPipeline->vulkanRenderPass,
            ClassManager::getDynamicClassMetadata(this).mClassDefinition.mName,
            HashedString(std::to_string(renderer->getShaderInstance()->mShader->getID())),
            uniformBuffers,
            instancedMeshRenderer->getGPUVertexBuffersContainer()
        };

        mGPUShaders.emplace(instancedMeshData, instancedMeshData.mShader->compileShader(shaderCompilationData));

        // setupShader(mGPUShaders.at(instancedMeshData));
        // bindShader(instancedMeshData);
    }
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
    WeakPtr<GPUShader> gpuShader = mGPUShaders.at(instancedMeshData);
    // gpuShader->bindUniformBuffer(GET_SYSTEM(ShaderManager).getShaderPropertiesGPUUniformBuffer(instancedMeshData.mShader));
    
    // WeakPtr<Model> model = GET_SYSTEM(ModelManager).getModelFromMesh(instancedMeshData.mMesh);
    // if(model)
    // {
    //     WeakPtr<GPUSkeletonState> skeletonState = model->getSkeletonState();
    //     if(skeletonState)
    //     {
    //         gpuShader->bindUniformBuffer(GET_SYSTEM(GPUSkeletalAnimationManager).getSkeletonRenderStateGPUUniformBuffer(skeletonState));
    //     }
    // }

    // gpuShader->bindUniformBuffer(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData));
    // gpuShader->bindUniformBuffer(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices));

    // mGPUShaders.at(instancedMeshData)->bindTextures(mGPUShaders.at(instancedMeshData)->getGPUShader(), GET_SYSTEM(ShaderManager).getShaderTextureBindings(instancedMeshData.mShader));
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
    WeakPtr<GPUShader> gpuShader = mGPUShaders.at(instancedMeshData);

    gpuShader->enable();
    instancedMeshRenderer->render();
    gpuShader->disable();
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

    {
        PROFILER_GPU_NAMED(renderPass, mRenderPipeline->vulkanRenderPass->mGPUContext->mTracyContext, mRenderPipeline->vulkanRenderPass->mGPUContext->vulkanCommandBuffers[mRenderPipeline->vulkanRenderPass->mGPUContext->currentFrame]->getVkCommandBuffer())
        render();
    }
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