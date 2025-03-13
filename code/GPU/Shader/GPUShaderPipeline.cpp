#include "GPU/Shader/GPUShaderPipeline.h"
#include "GPU/Core/GPULog.h"

void GPUShaderPipeline::init(const GPUShaderPipelineData& gpuGPUShaderPipelineData, GPURenderPass* renderPass, Ptr<GPUContext> gpuContext)
{
    mGPUShaderPipelineData = gpuGPUShaderPipelineData;
    mRenderPass = renderPass;
    mGPUContext = gpuContext;

    mGPUShaderDescriptorSets = OwnerPtr<GPUShaderDescriptorSets>::newObject();
    mGPUShaderDescriptorSets->init(mGPUShaderPipelineData.mGPUShaderDescriptorSetsData, mGPUContext);

    mGPUVertexInputData.mVertexInputBindingDescriptions.resize(mGPUShaderPipelineData.mVertexInputBuffers.size());
    mGPUVertexInputData.mVertexInputAttributeDescriptions.resize(mGPUShaderPipelineData.mVertexInputBuffers.size());
    FOR_ARRAY(i, mGPUShaderPipelineData.mVertexInputBuffers)
    {
        const GPUVertexBuffer& gpuVertexBuffer = mGPUShaderPipelineData.mVertexInputBuffers[i];
        mGPUVertexInputData.mVertexInputBindingDescriptions[i].binding = i;
        mGPUVertexInputData.mVertexInputBindingDescriptions[i].stride = gpuVertexBuffer.mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes;
        mGPUVertexInputData.mVertexInputBindingDescriptions[i].inputRate = gpuVertexBuffer.mData.mInstanceDivisor == 0 ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
    
        mGPUVertexInputData.mVertexInputAttributeDescriptions[i].binding = i;
        mGPUVertexInputData.mVertexInputAttributeDescriptions[i].location = i;
        VkFormat format = VK_FORMAT_UNDEFINED;
        switch (gpuVertexBuffer.mData.mGPUVariableData.mGPUDataType.mPrimitiveDataType)
        {
        case GPUPrimitiveDataType::FLOAT:
            switch (gpuVertexBuffer.mData.mGPUVariableData.mGPUDataType.getSizePrimitiveType())
            {
            case 1:
                format = VK_FORMAT_R32_SFLOAT;
                break;
            case 2:
                format = VK_FORMAT_R32G32_SFLOAT;
                break;
            case 3:
                format = VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case 4:
                format = VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
            default:
                CHECK_MSG(false, "ERROR")
                break;
            }
            break;
        case GPUPrimitiveDataType::INT:
            switch (gpuVertexBuffer.mData.mGPUVariableData.mGPUDataType.getSizePrimitiveType())
            {
            case 1:
                format = VK_FORMAT_R32_SINT;
                break;
            case 2:
                format = VK_FORMAT_R32G32_SINT;
                break;
            case 3:
                format = VK_FORMAT_R32G32B32_SINT;
                break;
            case 4:
                format = VK_FORMAT_R32G32B32A32_SINT;
                break;
            default:
                CHECK_MSG(false, "ERROR")
                break;
            }
            break;
         case GPUPrimitiveDataType::UNSIGNED_INT:
            switch (gpuVertexBuffer.mData.mGPUVariableData.mGPUDataType.getSizePrimitiveType())
            {
            case 1:
                format = VK_FORMAT_R32_UINT;
                break;
            case 2:
                format = VK_FORMAT_R32G32_UINT;
                break;
            case 3:
                format = VK_FORMAT_R32G32B32_UINT;
                break;
            case 4:
                format = VK_FORMAT_R32G32B32A32_UINT;
                break;
            default:
                CHECK_MSG(false, "ERROR")
                break;
            }
            break;
        
        default:
            CHECK_MSG(false, "ERROR")
            break;
        }

        mGPUVertexInputData.mVertexInputAttributeDescriptions[i].format = format;//VK_FORMAT_R32G32B32_SFLOAT;
        // mGPUVertexInputData.mVertexInputAttributeDescriptions[i].offset = offsetof(Vertex, position);
    }
}

void GPUShaderPipeline::terminate()
{
    vertexShader.terminate();
    fragmentShader.terminate();

    vkDestroyPipeline(mGPUContext->vulkanDevice->getDevice(), mPipeline, ALLOCATOR);
    GPU_LOG("Destroyed Vulkan graphics pipeline");
    vkDestroyPipelineLayout(mGPUContext->vulkanDevice->getDevice(), mPipelineLayout, ALLOCATOR);
    GPU_LOG("Destroyed Vulkan graphics pipeline layout");

    mGPUShaderDescriptorSets->terminate();
    mGPUShaderDescriptorSets.invalidate();
}

void GPUShaderPipeline::bind(const GPUCommandBuffer& vulkanCommandBuffer) const
{
    vkCmdBindPipeline(vulkanCommandBuffer.getVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
}

void GPUShaderPipeline::enable() const
{
    const GPUCommandBuffer& vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];
    bind(vulkanCommandBuffer);

    VkDescriptorSet descriptorSet = mGPUShaderDescriptorSets->descriptorSets[mGPUContext->currentFrame];
    VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    VkPipelineLayout pipelineLayout = mPipelineLayout;
    constexpr u32 firstSet = 0;
    constexpr u32 descriptorSetCount = 1;
    constexpr u32 dynamicOffsetCount = 0;
    constexpr u32* dynamicOffsets = nullptr;
    vkCmdBindDescriptorSets(vulkanCommandBuffer.getVkCommandBuffer(), pipelineBindPoint, pipelineLayout, firstSet, descriptorSetCount, &descriptorSet, dynamicOffsetCount, dynamicOffsets);
}

void GPUShaderPipeline::disable() const
{
}

void GPUShaderPipeline::compile(const GPUShaderModuleData& vertex, const GPUShaderModuleData& fragment)
{
    PROFILER_CPU_NAMED(GPUShaderPipeline_compile)
    if (!vertexShader.init(mGPUContext, vertex))
    {
        CHECK_MSG(false, "Could not initialize vertex shader");
    }
    if (!fragmentShader.init(mGPUContext, fragment))
    {
        CHECK_MSG(false, "Could not initialize fragment shader");
    }

    VkPipelineShaderStageCreateInfo vertexGPUShaderStageInfo{};
    vertexGPUShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexGPUShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexGPUShaderStageInfo.module = vertexShader.getGPUShaderModule();
    vertexGPUShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentGPUShaderStageInfo{};
    fragmentGPUShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentGPUShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentGPUShaderStageInfo.module = fragmentShader.getGPUShaderModule();
    fragmentGPUShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {
            vertexGPUShaderStageInfo,
            fragmentGPUShaderStageInfo
    };
    
    VkPipelineVertexInputStateCreateInfo vertexInputState{};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.pVertexBindingDescriptions = mGPUVertexInputData.mVertexInputBindingDescriptions.data();
    vertexInputState.vertexBindingDescriptionCount = mGPUVertexInputData.mVertexInputBindingDescriptions.size();
    vertexInputState.pVertexAttributeDescriptions = mGPUVertexInputData.mVertexInputAttributeDescriptions.data();
    vertexInputState.vertexAttributeDescriptionCount = mGPUVertexInputData.mVertexInputAttributeDescriptions.size();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = (float) mGPUContext->vulkanSwapChain->getExtent().height;
    viewport.width = (float) mGPUContext->vulkanSwapChain->getExtent().width;
    viewport.height = -(float) mGPUContext->vulkanSwapChain->getExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = mGPUContext->vulkanSwapChain->getExtent();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.lineWidth = 1.0f;
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.depthBiasEnable = VK_FALSE;
    rasterizationState.depthBiasConstantFactor = 0.0f;
    rasterizationState.depthBiasClamp = 0.0f;
    rasterizationState.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.rasterizationSamples = mRenderPass->getGPURenderPassData().mSampleCountFlagBits;
    // multisampleState.pSampleMask = nullptr;
    // multisampleState.alphaToCoverageEnable = VK_FALSE;
    // multisampleState.alphaToOneEnable = VK_FALSE;

    /*
        * There are certain limitations of our current MSAA implementation which may impact the quality of the output image in more detailed scenes.
        * For example, we're currently not solving potential problems caused by shader aliasing, i.e. MSAA only smoothens out the edges of geometry but not the interior filling.
        * This may lead to a situation when you get a smooth polygon rendered on screen but the applied texture will still look aliased if it contains high contrasting colors.
        * One way to approach this problem is to enable Sample Shading which will improve the image quality even further, though at an additional performance cost:
        */
    multisampleState.sampleShadingEnable = VK_FALSE;
    //multisampleState.sampleShadingEnable = VK_TRUE; // enable sample shading in the pipeline
    multisampleState.minSampleShading = 1.0f;
    //multisampleState.minSampleShading = .2f; // min fraction for sample shading; closer to one is smoother

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = VK_LOGIC_OP_COPY;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachmentState;
    colorBlendState.blendConstants[0] = 0.0f;
    colorBlendState.blendConstants[1] = 0.0f;
    colorBlendState.blendConstants[2] = 0.0f;
    colorBlendState.blendConstants[3] = 0.0f;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = mGPUShaderPipelineData.mGPUShaderPipelineDepthStencilData.mDepthTestEnable;
    depthStencil.depthWriteEnable = mGPUShaderPipelineData.mGPUShaderPipelineDepthStencilData.mDepthWriteEnable;
    depthStencil.depthCompareOp = mGPUShaderPipelineData.mGPUShaderPipelineDepthStencilData.mDepthCompareOp;
    depthStencil.depthBoundsTestEnable = mGPUShaderPipelineData.mGPUShaderPipelineDepthStencilData.mDepthBoundsTestEnable;
    depthStencil.stencilTestEnable = mGPUShaderPipelineData.mGPUShaderPipelineDepthStencilData.mStencilTestEnable;
    depthStencil.front = mGPUShaderPipelineData.mGPUShaderPipelineDepthStencilData.mStencilFront;
    depthStencil.back = mGPUShaderPipelineData.mGPUShaderPipelineDepthStencilData.mStencilBack;
    depthStencil.minDepthBounds = mGPUShaderPipelineData.mGPUShaderPipelineDepthStencilData.mMinDepthBounds;
    depthStencil.maxDepthBounds = mGPUShaderPipelineData.mGPUShaderPipelineDepthStencilData.mMaxDepthBounds;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &mGPUShaderDescriptorSets->descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(mGPUContext->vulkanDevice->getDevice(), &pipelineLayoutInfo, ALLOCATOR, &mPipelineLayout) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan graphics pipeline layout");
    }
    GPU_LOG("Created Vulkan graphics pipeline layout");

    VkDynamicState dynamicStateEnables[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = NULL;
    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 2;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputState;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlendState;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = mPipelineLayout;
    pipelineInfo.renderPass = mRenderPass->getRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    constexpr int createInfoCount = 1;
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(mGPUContext->vulkanDevice->getDevice(), pipelineCache, createInfoCount, &pipelineInfo, ALLOCATOR, &mPipeline) != VK_SUCCESS) {
        CHECK_MSG(false,"Could not create Vulkan graphics pipeline");
    }
    GPU_LOG("Created Vulkan graphics pipeline");
}