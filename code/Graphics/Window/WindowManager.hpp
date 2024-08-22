#pragma once

#include "Core/ECS/System.hpp"
#include "Graphics/GPU/GPUWindow.hpp"

class WindowManager : public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void update() override;

    Ptr<GPUWindow> createWindow(const GPUWindowData& gpuWindowData);
    Ptr<GPUWindow> getWindow(u32 index) const;
private:
	std::vector<OwnerPtr<GPUWindow>> mWindows;
    Ptr<GPUWindow> mMainWindow;
public:
    GET(MainWindow)
};
REGISTER_CLASS(WindowManager);
