#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <functional>
#include <string>

namespace GPUAPI {

    struct Size {
        int Width;
        int Height;
    };

}

namespace GPUAPI {

    class App;

    class Window {
    public:
        struct Config {
            std::string Title;
            uint32_t Width;
            uint32_t Height;
        };

    private:
        struct UserPointer {
            std::function<void(int, int)> OnResize;
            std::function<void(bool)> OnMinimize;
        };

    private:
        Config config;
        GLFWwindow* glfwWindow;
        UserPointer userPointer;

    public:
        explicit Window(Config config);

        GLFWwindow* getGlfwWindow() const;

        bool initialize();

        void terminate();

        void setOnResize(const std::function<void(int, int)>& onResized);

        void setOnMinimize(const std::function<void(bool)>& onMinimize);

        Size getSizeInPixels() const;

        void getSizeInPixels(int* width, int* height) const;

        Size getSizeInScreenCoordinates() const;

        bool shouldClose() const;

        void pollEvents() const;

        void waitUntilNotMinimized() const;

    private:
        bool isIconified() const;

        static void onFramebufferSizeChange(GLFWwindow* glfWwindow, int width, int height);

        static void onWindowIconifyChange(GLFWwindow* glfWwindow, int iconified);

        static void onKeyChange(GLFWwindow* glfwWindow, int key, int scanCode, int action, int mods);
    };
}