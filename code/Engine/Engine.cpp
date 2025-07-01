#include "Engine/Engine.hpp"
#include "Engine/EngineConfig.hpp"
#include "Engine/Command/CommandLine.hpp"
#include "Engine/Time/TimerManager.hpp"
#include "Engine/Input/Input.hpp"
#include "Engine/Events/EventsManager.hpp"
#include "Graphics/Module.hpp"
#include "Scripting/ScriptEngine.hpp"

#include "Scene/ScenesManager.hpp"
#include "UI/UIManager.hpp"
#include "Engine/Time/TimeUtils.hpp"

using namespace std::chrono_literals;

void Engine::init()
{
	mFPS = 60;

	Core::Log::init();

	Core::Memory::init();
	Core::Profiler::init();
	Event::EventsManager::getInstance().init();
    ECManager.init();
    System::SystemsManager::getInstance().init();

    CREATE_SYSTEM(Time::Time);
    CREATE_SYSTEM(EngineConfig);
    // CREATE_SYSTEM(GPUInterface);
    CREATE_SYSTEM(Window::WindowManager);
    Window::WindowData windowData;
    windowData.mTitle = "Vulkan Engine";
    windowData.mFullScreen = false;
    windowData.mWindowSize.set(800, 600);
    windowData.mMainWindow = true;
    GET_SYSTEM(Window::WindowManager).createWindow(windowData);
    CREATE_SYSTEM(GPUInstance);
    CREATE_SYSTEM(Input::Input);
    GET_SYSTEM(Input::Input).setWindowInputAdapter(GET_SYSTEM(Window::WindowManager).getMainWindow());
    CREATE_SYSTEM(Time::TimerManager);
    CREATE_SYSTEM(GPUMeshFactory);
    CREATE_SYSTEM(GPUShaderManager);
    CREATE_SYSTEM(CameraManager);
    CREATE_SYSTEM(GPUSkeletalAnimationManager);
    CREATE_SYSTEM(ModelManager);
    CREATE_SYSTEM(RenderEngine);
    CREATE_SYSTEM(DebugRenderer);
    CREATE_SYSTEM(UIManager);
    CREATE_SYSTEM(ScenesManager);
    CREATE_SYSTEM(Command::CommandLine);
    CREATE_SYSTEM(ScriptEngine);
}

void Engine::preSceneChanged()
{
	GET_SYSTEM(ScriptEngine).preSceneChanged();
	GET_SYSTEM(RenderEngine).preSceneChanged();
	GET_SYSTEM(Time::TimerManager).terminate();
}

void Engine::postSceneChanged()
{
	GET_SYSTEM(RenderEngine).postSceneChanged();
	GET_SYSTEM(ScriptEngine).postSceneChanged();
}

void Engine::run()
{
	Core::f32 inverseFPS = 1.0f / mFPS;
	Core::f32 inverseFPSMillis = inverseFPS * 1000.0f;

	Core::f32 diff = 0;

	while (!GET_SYSTEM(Window::WindowManager).getMainWindow()->isClosed())
	{
        //FrameMarkStart("frame");
		GET_SYSTEM(Time::Time).startFrame();

		if (GET_SYSTEM(ScenesManager).pendingLoadRequests())
		{
			preSceneChanged();
			GET_SYSTEM(ScenesManager).loadPendingScenes();
			postSceneChanged();
		}

		GET_SYSTEM(Input::Input).update();
		GET_SYSTEM(Window::WindowManager).getMainWindow()->pollEvents();

		GET_SYSTEM(Command::CommandLine).update();

		GET_SYSTEM(ScenesManager).update();
		GET_SYSTEM(Time::TimerManager).update();
		GET_SYSTEM(ScriptEngine).update();
		GET_SYSTEM(RenderEngine).update();

		Core::f32 dtMillis = GET_SYSTEM(Time::Time).getElapsedTimeMillis();
		
		if (inverseFPSMillis >= dtMillis)
		{
			diff = inverseFPSMillis - dtMillis;
			auto diff_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double, std::milli>(diff));
			std::this_thread::sleep_for(std::chrono::milliseconds(diff_duration.count()));
		}
		
		GET_SYSTEM(Time::Time).endFrame();
        //FrameMarkEnd("frame");
	}
}

void Engine::terminate()
{

	Core::MemoryTracking::log();
	System::SystemsManager::getInstance().terminate();
	System::SystemsManager::deleteInstance();
    ECManager.terminate();
    EC::EntityComponentManager::deleteInstance();
	Event::EventsManager::getInstance().terminate();
	Event::EventsManager::deleteInstance();
	Core::Profiler::terminate();
	Core::Memory::terminate();
    Core::HashedStringsManager::terminate();

    LOG("Terminated OK!")

	Core::Log::terminate();
}
