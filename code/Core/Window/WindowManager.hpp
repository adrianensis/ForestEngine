#pragma once

#include "Core/ECS/System.hpp"
#include "Core/Window/Window.hpp"

class WindowManager : public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();

    Ptr<Window> createWindow(const WindowData& windowData);
    Ptr<Window> getWindow(u32 index) const;
private:
	std::vector<OwnerPtr<Window>> mWindows;
    Ptr<Window> mMainWindow;
public:
    GET(MainWindow)
};
REGISTER_CLASS(WindowManager);
