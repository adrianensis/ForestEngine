#include "Window/WindowManager.hpp"

NS_BEGIN(Window)
void WindowManager::init()
{
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

Core::WeakPtr<Window> WindowManager::createWindow(const WindowData& windowData)
{
    Core::WeakPtr<Window> window = mWindows.emplace_back(Core::OwnerPtr<Window>::newObject());
    window->init(mWindows.size() - 1, windowData);

    if(windowData.mMainWindow)
    {
        mMainWindow = window;
    }

    return window;
}

Core::WeakPtr<Window> WindowManager::getWindow(Core::u32 index) const
{
    Core::WeakPtr<Window> window = mWindows.at(index);
    return window;
}
NS_END