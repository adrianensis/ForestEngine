#include "Graphics/GPU/GPUGraphicsPipeline.h"

#include "Graphics/GPU/Vertex.h"

#include "Core/Minimal.hpp"
//namespace GPUAPI {

    const VkAllocationCallbacks* GPUGraphicsPipeline::ALLOCATOR = VK_NULL_HANDLE;

    GPUGraphicsPipeline::GPUGraphicsPipeline(GPURenderPass* vulkanRenderPass, GPUSwapChain* vulkanSwapChain, GPUDevice* vulkanDevice, GPUPhysicalDevice* vulkanPhysicalDevice)
        : vulkanRenderPass(vulkanRenderPass), vulkanSwapChain(vulkanSwapChain), vulkanDevice(vulkanDevice), vulkanPhysicalDevice(vulkanPhysicalDevice) {
    }

    const VkPipeline GPUGraphicsPipeline::getPipeline() const {
        return pipeline;
    }

    const VkPipelineLayout GPUGraphicsPipeline::getPipelineLayout() const {
        return pipelineLayout;
    }

    bool GPUGraphicsPipeline::initialize(const GPUShader& vertexShader, const GPUShader& fragmentShader, VkDescriptorSetLayout descriptorSetLayout) {

        VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
        vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStageInfo.module = vertexShader.getShaderModule();
        vertexShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
        fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStageInfo.module = fragmentShader.getShaderModule();
        fragmentShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {
                vertexShaderStageInfo,
                fragmentShaderStageInfo
        };

        VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.pVertexBindingDescriptions = &bindingDescription;
        vertexInputState.vertexBindingDescriptionCount = 1;
        vertexInputState.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputState.vertexAttributeDescriptionCount = attributeDescriptions.size();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
        inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyState.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) vulkanSwapChain->getExtent().width;
        viewport.height = (float) vulkanSwapChain->getExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = vulkanSwapChain->getExtent();

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
        multisampleState.rasterizationSamples = vulkanPhysicalDevice->getSampleCount();
        multisampleState.pSampleMask = nullptr;
        multisampleState.alphaToCoverageEnable = VK_FALSE;
        multisampleState.alphaToOneEnable = VK_FALSE;

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
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(vulkanDevice->getDevice(), &pipelineLayoutInfo, ALLOCATOR, &pipelineLayout) != VK_SUCCESS) {
            CHECK_MSG(false,"Could not create Vulkan graphics pipeline layout");
            return false;
        }
        VULKAN_LOG("Created Vulkan graphics pipeline layout");

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
        pipelineInfo.pDynamicState = nullptr;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = vulkanRenderPass->getRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        constexpr int createInfoCount = 1;
        VkPipelineCache pipelineCache = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(vulkanDevice->getDevice(), pipelineCache, createInfoCount, &pipelineInfo, ALLOCATOR, &pipeline) != VK_SUCCESS) {
            CHECK_MSG(false,"Could not create Vulkan graphics pipeline");
            return false;
        }
        VULKAN_LOG("Created Vulkan graphics pipeline");

        return true;
    }

    void GPUGraphicsPipeline::terminate() {
        vkDestroyPipeline(vulkanDevice->getDevice(), pipeline, ALLOCATOR);
        VULKAN_LOG("Destroyed Vulkan graphics pipeline");
        vkDestroyPipelineLayout(vulkanDevice->getDevice(), pipelineLayout, ALLOCATOR);
        VULKAN_LOG("Destroyed Vulkan graphics pipeline layout");
    }

    void GPUGraphicsPipeline::bind(const GPUCommandBuffer& vulkanCommandBuffer) const {
        vkCmdBindPipeline(vulkanCommandBuffer.getVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    }
// }
