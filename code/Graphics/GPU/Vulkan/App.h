#pragma once

#include "Log.h"
#include "FileSystem.h"
#include "Window.h"
#include "Vulkan.h"
#include "GPUPhysicalDevice.h"
#include "GPUDevice.h"
#include "GPUCommandPool.h"
#include "GPUCommandBuffer.h"
#include "GPUVertexBuffer.h"
#include "GPUIndexBuffer.h"
#include "GPUUniformBuffer.h"
#include "GPUSwapChain.h"
#include "GPURenderPass.h"
#include "GPUGraphicsPipeline.h"
#include "GPUFramebuffer.h"
#include "GPUImage.h"
#include "Vertex.h"
#include "Core/Maths/Matrix4.hpp"
#include <vulkan/vulkan.h>

#include <vector>

namespace GPUAPI {

    struct CameraUniform {
        alignas(16) Matrix4 model;
        alignas(16) Matrix4 view;
        alignas(16) Matrix4 projection;
    };

    class App {
    public:
        struct Config {
            std::string Name;
            Log::Level LogLevel;
            Window::Config mWindow;
            Vulkan::Config mVulkan;
        };

    private:
        Config config;
        FileSystem* fileSystem;
        Window* window;
        Vulkan* vulkan;
        GPUPhysicalDevice* vulkanPhysicalDevice;
        GPUDevice* vulkanDevice;
        GPUCommandPool* vulkanCommandPool;
        std::vector<GPUCommandBuffer> vulkanCommandBuffers;
        GPUShader* vertexShader;
        GPUShader* fragmentShader;
        GPUVertexBuffer* vulkanVertexBuffer;
        GPUIndexBuffer* vulkanIndexBuffer;
        GPUSwapChain* vulkanSwapChain;
        GPURenderPass* vulkanRenderPass;
        GPUGraphicsPipeline* vulkanGraphicsPipeline;
        std::vector<GPUFramebuffer> framebuffers;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        uint32_t currentFrame = 0;
        bool windowResized = false;

        std::vector<GPUUniformBuffer> uniformBuffers;
        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> descriptorSets;

        GPUImage* vulkanTextureImage;
        VkImageView textureImageView = VK_NULL_HANDLE;
        VkSampler textureSampler = VK_NULL_HANDLE;

        GPUImage* vulkanDepthImage;
        VkImageView depthImageView = VK_NULL_HANDLE;

        const std::string MODEL_PATH = "models/viking_room.obj";
        const std::string TEXTURE_PATH = "textures/viking_room.png";

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        uint32_t mipLevels = 0;

        GPUImage* vulkanColorImage;
        VkImageView colorImageView = VK_NULL_HANDLE;

    public:
        explicit App(Config config);

        ~App();

        void run();

    private:
        bool initialize();

        bool loadModel();

        bool initializeColorResources();

        bool initializeDepthResources();

        VkFormat findDepthFormat();

        bool hasStencilComponent(VkFormat format) const;

        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

        bool initializeTextureImage();

        bool initializeTextureImageView();

        bool initializeTextureSampler();

        bool transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) const;

        bool generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) const;

        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;

        VkCommandBuffer beginSingleTimeCommands() const;

        void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

        bool initializeUniformBuffers();

        bool initializeDescriptorSetLayout();

        bool initializeDescriptorPool();

        bool initializeDescriptorSets();

        bool initializeRenderingObjects();

        bool initializeFramebuffers();

        bool initializeSyncObjects();

        void terminate();

        void terminateSyncObjects() const;

        void terminateFramebuffers();

        void terminateColorResources();

        void terminateDepthResources();

        void terminateRenderingObjects();

        void terminateUniformBuffers();

        bool recreateRenderingObjects();

        void drawFrame();

        void update();
    };

}
