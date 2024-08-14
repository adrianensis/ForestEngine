#include "App.h"
#include "Environment.h"
#include "Log.h"

int main()
{
    GPUAPI::App::Config config{};
    config.Name = "GPUAPI";
    config.LogLevel = GPUAPI::Log::Level::Debug;
    config.mWindow.Title = config.Name;
    config.mWindow.Width = 800;
    config.mWindow.Height = 600;
    config.mVulkan.Name = config.Name;
#ifdef VD_BUILD_DEBUG
    config.mVulkan.ValidationLayersEnabled = true;
#endif

    auto* app = new GPUAPI::App(config);
    app->run();
    delete app;
}