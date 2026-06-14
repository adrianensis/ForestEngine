#pragma once

#include "Core/Memory/Pointers.hpp"
#include "Core/System/System.hpp"
#include "Window/Window.hpp"
NS_BEGIN(Window)
class WindowManager : public System::System
{
public:
    void init();
    virtual void terminate() override;
    void update();

    // Core::WeakPtr<IWindow> createWindow(const WindowData& windowData);

    template<class W>
    requires std::derived_from<W, IWindow>
    Core::WeakPtr<IWindow> createWindow(const WindowData& windowData)
    {
        Core::WeakPtr<IWindow> window = mWindows.emplace_back(Core::OwnerPtr<IWindow>::moveCast(Core::OwnerPtr<W>::newObject()));
        window->init(mWindows.size() - 1, windowData);

        if(windowData.mMainWindow)
        {
            mMainWindow = window;
        }

        return window;
    }

    Core::WeakPtr<IWindow> getWindow(Core::u32 index) const;
private:
	std::vector<Core::OwnerPtr<IWindow>> mWindows;
    Core::WeakPtr<IWindow> mMainWindow;
public:
    GET(MainWindow)
};
REGISTER_CLASS(WindowManager, System);
NS_END