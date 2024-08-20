#pragma once

#include "Log.h"
// #include "Window.h"
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
            // Window::Config mWindow;
            Vulkan::Config mVulkan;
        };

    private:
        Config config;
        Window* window;
        GPUShader* vertexShader;
        GPUShader* fragmentShader;
        GPUIndexBuffer* vulkanIndexBuffer;
        bool windowResized = false;


        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

    public:
        explicit App(Config config);

        ~App();

        void run();

    private:
        bool initialize();

        bool initializeRenderingObjects();

        void terminate();

        void drawFrame();

        void update();
    };

}
