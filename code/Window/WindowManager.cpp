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
        mWindows.at(i)->update();
    }
}

Core::WeakPtr<IWindow> WindowManager::getWindow(Core::u32 index) const
{
    Core::WeakPtr<IWindow> window = mWindows.at(index);
    return window;
}
NS_END