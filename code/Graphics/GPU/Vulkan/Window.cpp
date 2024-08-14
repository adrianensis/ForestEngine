#include "Window.h"

#include <utility>
#include "Log.h"

namespace GPUAPI {

    Window::Window(Config config) : config(std::move(config)), glfwWindow(nullptr) {
    }

    GLFWwindow* Window::getGlfwWindow() const {
        return glfwWindow;
    }

    Size Window::getSizeInPixels() const {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(glfwWindow, &width, &height);
        return { width, height };
    }

    void Window::getSizeInPixels(int* width, int* height) const {
        glfwGetFramebufferSize(glfwWindow, width, height);
    }

    Size Window::getSizeInScreenCoordinates() const {
        int width;
        int height;
        glfwGetWindowSize(glfwWindow, &width, &height);
        return { width, height };
    }

    void Window::setOnResize(const std::function<void(int, int)>& onResized) {
        userPointer.OnResize = onResized;
    }

    void Window::setOnMinimize(const std::function<void(bool)>& onMinimize) {
        userPointer.OnMinimize = onMinimize;
    }

    bool Window::initialize() {
        bool glfwInitialized = glfwInit();
        if (!glfwInitialized) {
            VD_LOG_CRITICAL("Could not initialize GLFW");
            return false;
        }
        VD_LOG_INFO("Initialized GLFW");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        GLFWmonitor* fullscreenMonitor = nullptr;
        GLFWwindow* sharedWindow = nullptr;
        glfwWindow = glfwCreateWindow((int32_t) config.Width, (int32_t) config.Height, config.Title.c_str(), fullscreenMonitor, sharedWindow);
        if (!glfwWindow) {
            VD_LOG_CRITICAL("Could not create GLFW window");
            return false;
        }
        VD_LOG_INFO("Created GLFW window");

        glfwSetWindowUserPointer(glfwWindow, &userPointer);
        glfwSetFramebufferSizeCallback(glfwWindow, onFramebufferSizeChange);
        glfwSetWindowIconifyCallback(glfwWindow, onWindowIconifyChange);
        glfwSetKeyCallback(glfwWindow, onKeyChange);

        return true;
    }

    void Window::terminate() {
        glfwDestroyWindow(glfwWindow);
        VD_LOG_INFO("Destroyed GLFW window");
        glfwTerminate();
        VD_LOG_INFO("Terminated GLFW");
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(glfwWindow);
    }

    void Window::pollEvents() const {
        glfwPollEvents();
    }

    void Window::waitUntilNotMinimized() const {
        int width = 0;
        int height = 0;
        getSizeInPixels(&width, &height);

        bool iconified = isIconified();

        while (width == 0 || height == 0 || iconified) {
            getSizeInPixels(&width, &height);
            iconified = isIconified();
            glfwWaitEvents();
        }
    }

    bool Window::isIconified() const {
        return glfwGetWindowAttrib(glfwWindow, GLFW_ICONIFIED) == 1;
    }

    void Window::onFramebufferSizeChange(GLFWwindow* glfWwindow, int width, int height) {
        auto userPointer = (UserPointer*) glfwGetWindowUserPointer(glfWwindow);
        userPointer->OnResize(width, height);
    }

    void Window::onWindowIconifyChange(GLFWwindow* glfWwindow, int iconified) {
        auto userPointer = (UserPointer*) glfwGetWindowUserPointer(glfWwindow);
        bool minimized = iconified == 1;
        userPointer->OnMinimize(minimized);
    }

    void Window::onKeyChange(GLFWwindow* glfwWindow, int key, int scanCode, int action, int mods) {
        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(glfwWindow, true);
        }
    }

}