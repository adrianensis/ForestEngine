#include "Graphics/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/RenderSharedContext.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Light/Light.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"

void BatchRenderer::init(const BatchData& batchData)
{
	mBatchData = batchData;
	mMeshBatcher.init(mBatchData.mMesh, mBatchData.mIsInstanced);
    initBuffers();

    if(mBatchData.mIsInstanced)
	{
        resizeMeshBuffers(1);
        setMeshBuffers(Ptr<const GPUMesh>::cast(mBatchData.mMesh));
	}

    mShader = ShaderUtils::createShader(mGPUVertexBuffersContainer, mGPUSharedBuffersContainer, mBatchData.mMaterial.get());
    
    bindSharedBuffers();

    mBatchData.mMaterial->bindToShader(mShader);
}

void BatchRenderer::terminate()
{
    mGPUVertexBuffersContainer.terminate();
    mGPUSharedBuffersContainer.terminate();
}

void BatchRenderer::bindSharedBuffers()
{
    mShader->bindSharedBuffer(GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData));
    mShader->bindSharedBuffer(GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices));

    if(mBatchData.mMaterial->getMaterialData().mReceiveLight)
    {
        mShader->bindSharedBuffer(GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mLightsData));
    }

    FOR_LIST(it, mGPUSharedBuffersContainer.getSharedBuffers())
    {
        mShader->bindSharedBuffer(*it);
    }
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
        drawCall();
        disable();
	}
}

void BatchRenderer::enable()
{
    mShader->enable();
    mGPUVertexBuffersContainer.enable();
    if(mBatchData.mMaterial->getMaterialData().mIsSkinned)
    {
        updateBoneTransforms();
    }
    mBatchData.mMaterial->enable();

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
    mGPUVertexBuffersContainer.disable();
    mShader->disable();
}

void BatchRenderer::onAddRenderer(Ptr<MeshRenderer> renderer)
{
	mRenderers.push_back(renderer);
	mRegenerateBuffersRequested = true;
}

void BatchRenderer::onRemoveRenderer(Ptr<MeshRenderer> renderer)
{
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
            newList.push_back(renderer);
        }
    }

    mRenderers.clear();
    mRenderers = newList;
    u32 newSize = mRenderers.size();

    mMeshBatcher.clear();
    if (newSize > mMaxMeshesThreshold)
    {
        PROFILER_BLOCK_CPU("new size");
		if(mMaxMeshesThreshold == 0)
		{
			mMaxMeshesThreshold = newSize;
		}
		else
		{
			mMaxMeshesThreshold += mMaxMeshesIncrement;
		}

        mMeshBatcher.resize(mMaxMeshesThreshold);
        resizeInstancedBuffers(mMaxMeshesThreshold);
        if(!mBatchData.mIsInstanced)
        {
            resizeMeshBuffers(mMaxMeshesThreshold);
        }
    	resizeIndicesBuffer(Ptr<const GPUMesh>::cast(mMeshBatcher.getInternalMesh()));

        PROFILER_END_BLOCK();
    }

    FOR_ARRAY(i, mRenderers)
    {
        mMeshBatcher.addInstanceData(mRenderers[i]->getMeshInstance(), mRenderers[i]->getRenderInstanceSlot().getSlot(), mRenderers[i]->getMaterialInstanceSlot().getSlot());
    }

    mDataSubmittedToGPU = false;
    mRegenerateBuffersRequested = false;
}

bool BatchRenderer::shouldRegenerateBuffers() const
{
    // TODO: possible optimization for dynamic objects: only regenerate buffers when transform changes.
	return mRegenerateBuffersRequested || !mBatchData.mIsStatic;
}

void BatchRenderer::updateBoneTransforms()
{	
	if(mBatchData.mMesh->mModel)
	{
		bool isAnimated = mBatchData.mMesh->mModel->isAnimated() && mBatchData.mMaterial->getMaterialData().mIsSkinned;
        if(isAnimated)
        {
            const std::vector<Matrix4>& transforms = GET_SYSTEM(AnimationManager).getBoneTransforms(mBatchData.mMesh->mModel);
            setBonesTransformsBuffer(transforms);
        }
	}
}

void BatchRenderer::initBuffers()
{
    PROFILER_CPU()

    bool isStatic = mBatchData.mIsStatic || mBatchData.mIsInstanced;
    mGPUVertexBuffersContainer.init();
    mGPUVertexBuffersContainer.setIndicesBuffer(GPUBuiltIn::PrimitiveTypes::mFace, isStatic);

    FOR_ARRAY(i, mBatchData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mBatchData.mMesh->mGPUVertexInputBuffers[i];
        GPUVertexBufferData bufferData(gpuVariableData);
        mGPUVertexBuffersContainer.createVertexBuffer(bufferData, isStatic);
    }

    GPUVertexBufferData bufferDataInstanceIDs(GPUBuiltIn::VertexInput::mInstanceID, mBatchData.mIsInstanced ? 1 : 0);
    mGPUVertexBuffersContainer.createVertexBuffer(bufferDataInstanceIDs, isStatic);

    GPUVertexBufferData bufferDataObjectIDs(GPUBuiltIn::VertexInput::mObjectID, mBatchData.mIsInstanced ? 1 : 0);
    mGPUVertexBuffersContainer.createVertexBuffer(bufferDataObjectIDs, isStatic);

    GPUVertexBufferData bufferDataMaterialInstanceIDs(GPUBuiltIn::VertexInput::mMaterialInstanceID, mBatchData.mIsInstanced ? 1 : 0);
    mGPUVertexBuffersContainer.createVertexBuffer(bufferDataMaterialInstanceIDs, isStatic);
    
    mGPUVertexBuffersContainer.disable();

    if(mBatchData.mMaterial->getMaterialData().mIsSkinned)
    {
        u32 bindingPointBoneMatrices = GET_SYSTEM(RenderSharedContext).requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mBonesMatrices.mType);
        mGPUSharedBuffersContainer.createSharedBuffer(bindingPointBoneMatrices, GPUBuiltIn::SharedBuffers::mBonesMatrices, isStatic);
        mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices).resize<Matrix4>(GPUBuiltIn::MAX_BONES);
    }
}

void BatchRenderer::resizeMeshBuffers(u32 maxInstances)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mBatchData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mBatchData.mMesh->mGPUVertexInputBuffers[i];
        mGPUVertexBuffersContainer.getVertexBuffer(gpuVariableData).resize(mBatchData.mMesh->mVertexCount * maxInstances);
    }
}

void BatchRenderer::resizeInstancedBuffers(u32 maxInstances)
{
    PROFILER_CPU()
    u32 bufferSizeMultiplier = mBatchData.mIsInstanced ? 1 : mBatchData.mMesh->mVertexCount;
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).resize(maxInstances * bufferSizeMultiplier);
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mObjectID).resize(maxInstances * bufferSizeMultiplier);
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mMaterialInstanceID).resize(maxInstances * bufferSizeMultiplier);
}

void BatchRenderer::setMeshBuffers(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mesh->mGPUVertexInputBuffers[i];
        mGPUVertexBuffersContainer.getVertexBuffer(gpuVariableData).setDataArray(mesh->mBuffers.at(gpuVariableData.mName));
    }
}

void BatchRenderer::setInstancedBuffers()
{
    PROFILER_CPU()
	mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).setDataArray(mMeshBatcher.getInstanceIDs());
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mObjectID).setDataArray(mMeshBatcher.getObjectIDs());
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mMaterialInstanceID).setDataArray(mMeshBatcher.getMaterialInstanceIDs());
}

void BatchRenderer::setBonesTransformsBuffer(const std::vector<Matrix4>& transforms)
{
    PROFILER_CPU()
    mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices).setDataArray(transforms);
}

void BatchRenderer::resizeIndicesBuffer(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    mGPUVertexBuffersContainer.getIndicesBuffer().resize(mesh->mIndices.size());
}

void BatchRenderer::setIndicesBuffer(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    mGPUVertexBuffersContainer.getIndicesBuffer().setDataArray(mesh->mIndices);
}

void BatchRenderer::drawCall()
{	
    PROFILER_CPU()
    if(!mDataSubmittedToGPU)
    {
        if(!mBatchData.mIsInstanced)
        {
            setMeshBuffers(Ptr<const GPUMesh>::cast(mMeshBatcher.getInternalMesh()));
        }
    	setIndicesBuffer(Ptr<const GPUMesh>::cast(mMeshBatcher.getInternalMesh()));
        setInstancedBuffers();
        mDataSubmittedToGPU = true;
    }

    GET_SYSTEM(GPUInterface).drawElements(GPUDrawPrimitive::TRIANGLES, mBatchData.mMesh->mIndices.size() * 3, mRenderers.size(), mBatchData.mIsInstanced);

}