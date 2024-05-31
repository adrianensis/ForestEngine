#include "Graphics/Renderer/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUState.hpp"
#include "Graphics/RenderState.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Light/Light.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/Model/SkeletalAnimation/SkeletalAnimationManager.hpp"

void BatchRenderer::init(HashedString label, const BatchData& batchData, Ptr<const Shader> customShader)
{
	mBatchData = batchData;
	mMeshBatcher.init(mBatchData.mMesh, mBatchData.mIsInstanced);
    initBuffers();

    if(mBatchData.mIsInstanced)
	{
        resizeMeshBuffers(1);
        setMeshBuffers(mBatchData.mMesh);
	}

    mGPUProgram = ShaderUtils::createShaderCustomFragment(label, mGPUVertexBuffersContainer, mGPUSharedBuffersContainer, getBatchData().mMaterial.get(), customShader);
    bindSharedBuffers();
    mBatchData.mMaterial->getShader()->bindTextures(mGPUProgram);
}

void BatchRenderer::terminate()
{
    mGPUVertexBuffersContainer.terminate();
    mGPUSharedBuffersContainer.terminate();
}

void BatchRenderer::bindSharedBuffers()
{
    mGPUProgram->bindSharedBuffer(GET_SYSTEM(MaterialManager).getMaterialPropertiesGPUSharedBuffer(mBatchData.mMaterial));
    if(GET_SYSTEM(SkeletalAnimationManager).getSkeletonStates().contains(mBatchData.mMesh->mModel))
    {
        Ptr<const SkeletonState> skeletonState = GET_SYSTEM(SkeletalAnimationManager).getSkeletonStates().at(mBatchData.mMesh->mModel);
        mGPUProgram->bindSharedBuffer(GET_SYSTEM(RenderState).getSkeletonRenderStateGPUSharedBuffer(skeletonState));
    }

    mGPUProgram->bindSharedBuffer(GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData));
    mGPUProgram->bindSharedBuffer(GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices));

    if(mBatchData.mMaterial->getMaterialData().mReceiveLight)
    {
        mGPUProgram->bindSharedBuffer(GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mLightsBufferData));
        mGPUProgram->bindSharedBuffer(GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mShadowMappingBufferData));
    }

    FOR_LIST(it, mGPUSharedBuffersContainer.getSharedBuffers())
    {
        mGPUProgram->bindSharedBuffer(*it);
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
    mGPUProgram->enable();
    mGPUVertexBuffersContainer.enable();
    mBatchData.mMaterial->getShader()->enable();
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
    mBatchData.mMaterial->getShader()->disable();
    mGPUVertexBuffersContainer.disable();
    mGPUProgram->disable();
}

void BatchRenderer::addRenderer(Ptr<MeshRenderer> renderer)
{
	mRenderers.push_back(renderer);
	mRegenerateBuffersRequested = true;
}

void BatchRenderer::removeRenderer(Ptr<MeshRenderer> renderer)
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
			mMaxMeshesThreshold += smMeshesIncrement;
		}

        mMeshBatcher.resize(mMaxMeshesThreshold);
        resizeInstancedBuffers(mMaxMeshesThreshold);
        if(!mBatchData.mIsInstanced)
        {
            resizeMeshBuffers(mMaxMeshesThreshold);
        }
    	resizeIndicesBuffer(mMeshBatcher.getInternalMesh());

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
}

void BatchRenderer::initBuffers()
{
    PROFILER_CPU()

    bool isStatic = mBatchData.mIsStatic || mBatchData.mIsInstanced;

    FOR_ARRAY(i, mBatchData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mBatchData.mMesh->mGPUVertexInputBuffers[i];
        GPUVertexBufferData bufferData(gpuVariableData);
        mGPUVertexBuffersContainer.addVertexBuffer(bufferData, isStatic);
    }

    GPUVertexBufferData bufferDataInstanceIDs(GPUBuiltIn::VertexInput::mInstanceID, mBatchData.mIsInstanced ? 1 : 0);
    mGPUVertexBuffersContainer.addVertexBuffer(bufferDataInstanceIDs, isStatic);

    GPUVertexBufferData bufferDataObjectIDs(GPUBuiltIn::VertexInput::mObjectID, mBatchData.mIsInstanced ? 1 : 0);
    mGPUVertexBuffersContainer.addVertexBuffer(bufferDataObjectIDs, isStatic);

    GPUVertexBufferData bufferDataMaterialInstanceIDs(GPUBuiltIn::VertexInput::mMaterialInstanceID, mBatchData.mIsInstanced ? 1 : 0);
    mGPUVertexBuffersContainer.addVertexBuffer(bufferDataMaterialInstanceIDs, isStatic);
    
    mGPUVertexBuffersContainer.create();
    
    mGPUVertexBuffersContainer.enable();
    mGPUVertexBuffersContainer.setIndicesBuffer(GPUBuiltIn::PrimitiveTypes::mFace, isStatic);
    mGPUVertexBuffersContainer.disable();
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
    if(!mRenderers.empty())
    {
        if(!mDataSubmittedToGPU)
        {
            if(!mBatchData.mIsInstanced)
            {
                setMeshBuffers(mMeshBatcher.getInternalMesh());
            }
            setIndicesBuffer(mMeshBatcher.getInternalMesh());
            setInstancedBuffers();
            mDataSubmittedToGPU = true;
        }

        GET_SYSTEM(GPUInterface).drawElements(GPUDrawPrimitive::TRIANGLES, mBatchData.mMesh->mIndices.size() * 3, mRenderers.size(), mBatchData.mIsInstanced);
    }
}