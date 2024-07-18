#pragma once

#include "Core/ECS/System.hpp"
#include "Graphics/Window/Window.hpp"

class WindowManager : public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void update() override;

    Ptr<Window> createWindow(const WindowData& windowData);
    Ptr<Window> getWindow(u32 index) const;
private:
	std::vector<OwnerPtr<Window>> mWindows;
    Ptr<Window> mMainWindow;
public:
    GET(MainWindow)
};
REGISTER_CLASS(WindowManager);
