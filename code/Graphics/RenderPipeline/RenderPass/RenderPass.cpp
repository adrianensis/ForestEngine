#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "Core/ECS/EntityHandler.hpp"

void RenderPass::init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData)
{
    mRenderPipeline = renderPipeline;
    mRenderPassData = renderPassData;

    if(mRenderPassData.mOutputFramebufferData.isValid())
    { 
        mOutputGPUFramebuffer.init(mRenderPassData.mOutputFramebufferData);
    }

    vulkanRenderPass = new GPURenderPass(GET_SYSTEM(GPUInstance).mGPUContext);
}

void RenderPass::terminate()
{
    vulkanRenderPass->terminate();
    delete vulkanRenderPass;
}

void RenderPass::addRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
	InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

	if (!mInstancedMeshRenderers.contains(instancedMeshData))
	{
        mInstancedMeshRenderers.insert(instancedMeshData);

        std::vector<GPUUniformBuffer> uniformBuffers;
        // uniformBuffers.push_back(GET_SYSTEM(MaterialManager).getMaterialPropertiesGPUUniformBuffer(instancedMeshData.mMaterial));
    
        Ptr<Model> model = GET_SYSTEM(ModelManager).getModelFromMesh(instancedMeshData.mMesh);
        if(model)
        {
            Ptr<GPUSkeletonState> skeletonState = model->getSkeletonState();
            if(skeletonState)
            {
                // uniformBuffers.push_back(GET_SYSTEM(GPUSkeletalAnimationManager).getSkeletonRenderStateGPUUniformBuffer(skeletonState));
            }
        }

        uniformBuffers.push_back(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData));
        // uniformBuffers.push_back(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices));

        Ptr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
        ShaderCompileData shaderCompileData
        {
            instancedMeshData.mMaterial,
            instancedMeshData.mMesh,
            vulkanRenderPass,
            ClassManager::getDynamicClassMetadata(this).mClassDefinition.mName,
            HashedString(std::to_string(renderer->getMaterialInstance()->mMaterial->getID())),
            uniformBuffers,
            instancedMeshRenderer->getGPUVertexBuffersContainer()
        };

        mGPUShaders.emplace(instancedMeshData, mRenderPassData.mShader->compileShader(shaderCompileData));

        // setupShader(mGPUShaders.at(instancedMeshData));
        // bindShader(instancedMeshData);
    }
}

void RenderPass::removeRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
    InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

    // Ptr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
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
    Ptr<GPUShader> gpuShader = mGPUShaders.at(instancedMeshData);
    // gpuShader->bindUniformBuffer(GET_SYSTEM(MaterialManager).getMaterialPropertiesGPUUniformBuffer(instancedMeshData.mMaterial));
    
    // Ptr<Model> model = GET_SYSTEM(ModelManager).getModelFromMesh(instancedMeshData.mMesh);
    // if(model)
    // {
    //     Ptr<GPUSkeletonState> skeletonState = model->getSkeletonState();
    //     if(skeletonState)
    //     {
    //         gpuShader->bindUniformBuffer(GET_SYSTEM(GPUSkeletalAnimationManager).getSkeletonRenderStateGPUUniformBuffer(skeletonState));
    //     }
    // }

    // gpuShader->bindUniformBuffer(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData));
    // gpuShader->bindUniformBuffer(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices));

    // mGPUShaders.at(instancedMeshData)->bindTextures(mGPUShaders.at(instancedMeshData)->getGPUShader(), GET_SYSTEM(MaterialManager).getMaterialTextureBindings(instancedMeshData.mMaterial));
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

void RenderPass::renderBatch(const InstancedMeshData& instancedMeshData)
{
    PROFILER_CPU()
    Ptr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
    Ptr<GPUShader> gpuShader = mGPUShaders.at(instancedMeshData);

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

    vulkanRenderPass->begin();
    render();
    vulkanRenderPass->end();
}

void RenderPass::updateGlobalData()
{
	PROFILER_CPU()

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);

    TypedComponentHandler<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 projectionViewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mProjectionMatrix : ortho;
    Matrix4 viewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mViewMatrix : Matrix4::smIdentity;

    projectionViewMatrix.mul(viewMatrix);

    GPUBuiltIn::UniformBuffers::GPUGlobalData gpuGlobalData =
    {
        projectionViewMatrix,
        camera->getOwnerEntity()->getFirstComponent<Transform>()->getWorldPosition()
    };
	GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData).setData(gpuGlobalData);
}

void RenderPass::setupShader(Ptr<Shader> shader) const
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
    if (!vulkanRenderPass->initialize())
    {
        CHECK_MSG(false, "Could not initialize Vulkan render pass");
    }
}