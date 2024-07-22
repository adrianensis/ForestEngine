#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/Material/MaterialManager.hpp"
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
}

void RenderPass::terminate()
{
}

void RenderPass::addRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
	InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

	if (!mInstancedMeshRenderers.contains(instancedMeshData))
	{
        mInstancedMeshRenderers.insert(instancedMeshData);
        mGPUPrograms.emplace(instancedMeshData, OwnerPtr<GPUProgram>());
      
        GPUVertexBuffersContainer gpuVertexBuffersContainer;
        instancedMeshData.mMesh->populateGPUVertexBuffersContainer(gpuVertexBuffersContainer, instancedMeshData.mIsStatic);

        Ptr<Shader> shader = getShader(instancedMeshData);
        mGPUPrograms.insert_or_assign(instancedMeshData, GET_SYSTEM(ShaderManager).compileShader(
            ClassManager::getDynamicClassMetadata(this).mClassDefinition.mName,
            gpuVertexBuffersContainer,
            instancedMeshData.mMaterial.get(),
            shader
        ));

    }
}

void RenderPass::removeRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
    InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

    Ptr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
    if(instancedMeshRenderer->isEmpty())
    {
        mInstancedMeshRenderers.erase(instancedMeshData);
    }
}

void RenderPass::preFramebufferEnabled()
{
}

void RenderPass::postFramebufferEnabled()
{
}

void RenderPass::bindShader(const InstancedMeshData& instancedMeshData)
{
    mGPUPrograms.at(instancedMeshData)->bindSharedBuffer(GET_SYSTEM(MaterialManager).getMaterialPropertiesGPUSharedBuffer(instancedMeshData.mMaterial));
    
    Ptr<Model> model = GET_SYSTEM(ModelManager).getModelFromMesh(instancedMeshData.mMesh);
    if(model)
    {
        Ptr<GPUSkeletonState> skeletonState = model->getSkeletonState();
        if(skeletonState)
        {
            mGPUPrograms.at(instancedMeshData)->bindSharedBuffer(GET_SYSTEM(GPUSkeletalAnimationManager).getSkeletonRenderStateGPUSharedBuffer(skeletonState));
        }
    }

    mGPUPrograms.at(instancedMeshData)->bindSharedBuffer(GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData));
    mGPUPrograms.at(instancedMeshData)->bindSharedBuffer(GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices));

    Ptr<Shader> shader = getShader(instancedMeshData);
    shader->bindTextures(mGPUPrograms.at(instancedMeshData), GET_SYSTEM(MaterialManager).getMaterialTextureBindings(instancedMeshData.mMaterial));
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
    Ptr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
    Ptr<Shader> shader = getShader(instancedMeshData);

    setupShader(shader);
    bindShader(instancedMeshData);

    mGPUPrograms.at(instancedMeshData)->enable();
    shader->enable();
    instancedMeshRenderer->render();
    shader->disable();
    mGPUPrograms.at(instancedMeshData)->disable();
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

    TypedComponentHandler<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 projectionViewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mProjectionMatrix : ortho;
    Matrix4 viewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mViewMatrix : Matrix4::smIdentity;

    projectionViewMatrix.mul(viewMatrix);

    GPUBuiltIn::SharedBuffers::GPUGlobalData gpuGlobalData =
    {
        projectionViewMatrix,
        camera->getOwnerEntity()->getFirstComponent<Transform>()->getWorldPosition()
    };
	GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).setData(gpuGlobalData);
}

Ptr<Shader> RenderPass::getShader(const InstancedMeshData& instancedMeshData) const
{
    return instancedMeshData.mMaterial->getShader();
}

void RenderPass::setupShader(Ptr<Shader> shader) const
{
    FOR_ARRAY(i, mRenderPassData.mDependencies)
    {
        FramebufferBinding framebufferBinding
        {
            mRenderPassData.mDependencies[i].mSamplerName,
            mRenderPassData.mDependencies[i].mRenderPass->getOutputGPUFramebuffer().getAttachments().at(mRenderPassData.mDependencies[i].mAttachmentType).mAttachmentID,
            mRenderPassData.mDependencies[i].mStage
        };
        shader->addFramebufferBinding(framebufferBinding);
    }
}