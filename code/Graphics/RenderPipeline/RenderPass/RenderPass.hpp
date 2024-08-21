#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshRenderer.hpp"
#include "Graphics/GPU/GPUFramebuffer.hpp"
#include "Graphics/Material/Shader/Shader.hpp"
#include "Core/ECS/ComponentsManager.hpp"

#include "Graphics/GPU/GPUFramebuffer.hpp"
#include "Graphics/GPU/GPURenderPass.h"
#include "Graphics/GPU/GPUGraphicsPipeline.h"
#include "Graphics/GPU/GPUImage.h"

class MeshRenderer;
class RenderPipeline;
class RenderPass;

class RenderPassDependency
{
public:
    HashedString mSamplerName;
    GPUFramebufferAttachmentType mAttachmentType;
    Ptr<RenderPass> mRenderPass;
    GPUPipelineStage mStage = GPUPipelineStage::NONE;
};

class RenderPassData
{
public:
    GeometricSpace mGeometricSpace = GeometricSpace::WORLD;
    std::vector<RenderPassDependency> mDependencies;
    GPUFramebufferData mOutputFramebufferData;
};

class RenderPass: public ObjectBase
{
public:
    virtual void init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData);
    void terminate();
    void addRenderer(TypedComponentHandler<MeshRenderer> renderer);
    void removeRenderer(TypedComponentHandler<MeshRenderer> renderer);
    virtual void renderPass();
protected:
    virtual void preFramebufferEnabled();
    virtual void postFramebufferEnabled();
    virtual void bindShader(const InstancedMeshData& instancedMeshData);
    virtual void preRender();
    virtual void renderBatch(const InstancedMeshData& instancedMeshData);
    virtual void render();
    virtual void postRender();
    virtual void updateGlobalData();
    virtual Ptr<Shader> getShader(const InstancedMeshData& instancedMeshData) const;
    virtual void setupShader(Ptr<Shader> shader) const;

private:
    bool initializeColorResources();
    bool initializeDepthResources();
    VkFormat findDepthFormat();
    bool initializeFramebuffers();

protected:
	std::unordered_set<InstancedMeshData, InstancedMeshData::InstancedMeshDataFunctor> mInstancedMeshRenderers;
    RenderPassData mRenderPassData;
    GPUFramebuffer mOutputGPUFramebuffer;
    Ptr<RenderPipeline> mRenderPipeline;

    GPURenderPass* vulkanRenderPass;
    GPUGraphicsPipeline* vulkanGraphicsPipeline;
    std::vector<GPUFramebuffer> framebuffers;
    GPUImage* vulkanDepthImage;
    VkImageView depthImageView = VK_NULL_HANDLE;
    GPUImage* vulkanColorImage;
    VkImageView colorImageView = VK_NULL_HANDLE;
public:
    CRGET(RenderPassData)
    CRGET(OutputGPUFramebuffer)
};
REGISTER_CLASS(RenderPass);