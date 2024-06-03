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

Ptr<Window> WindowManager::createWindow(const WindowData& windowData, bool setMain)
{
    Ptr<Window> window = mWindows.emplace_back(OwnerPtr<Window>::newObject());
    window->init(mWindows.size() - 1, windowData);

    if(setMain)
    {
        mMainWindow = window;
    }

    return window;
}

Ptr<Window> WindowManager::getWindow(u32 index) const
{
    Ptr<Window> window = mWindows.at(index);
    return window;
}