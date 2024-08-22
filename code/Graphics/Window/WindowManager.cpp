#include "Graphics/Window/WindowManager.hpp"

void WindowManager::init()
{
	LOG_TRACE()

	glfwInit();
}

void WindowManager::terminate()
{
    FOR_ARRAY(i, mWindows)
    {
        mWindows.at(i)->terminate();
    }
}

void WindowManager::update()
{
    FOR_ARRAY(i, mWindows)
    {
        mWindows.at(i)->pollEvents();
    }
}

Ptr<GPUWindow> WindowManager::createWindow(const GPUWindowData& gpuWindowData)
{
    Ptr<GPUWindow> gpuWindow = mWindows.emplace_back(OwnerPtr<GPUWindow>::newObject());
    gpuWindow->init(mWindows.size() - 1, gpuWindowData);

    if(gpuWindowData.mMainWindow)
    {
        mMainWindow = gpuWindow;
    }

    return gpuWindow;
}

Ptr<GPUWindow> WindowManager::getWindow(u32 index) const
{
    Ptr<GPUWindow> gpuWindow = mWindows.at(index);
    return gpuWindow;
}