#include "Core/Window/WindowManager.hpp"

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

WeakPtr<Window> WindowManager::createWindow(const WindowData& windowData)
{
    WeakPtr<Window> window = mWindows.emplace_back(OwnerPtr<Window>::newObject());
    window->init(mWindows.size() - 1, windowData);

    if(windowData.mMainWindow)
    {
        mMainWindow = window;
    }

    return window;
}

WeakPtr<Window> WindowManager::getWindow(u32 index) const
{
    WeakPtr<Window> window = mWindows.at(index);
    return window;
}