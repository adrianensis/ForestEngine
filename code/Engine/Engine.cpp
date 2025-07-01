#include "Engine/Engine.hpp"
#include "Engine/EngineConfig.hpp"
#include "Core/Command/CommandLine.hpp"
#include "Core/Time/TimerManager.hpp"
#include "Core/Input/Input.hpp"
#include "Core/Events/EventsManager.hpp"
#include "Graphics/Module.hpp"
#include "Scripting/Module.hpp"

#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Core/Time/TimeUtils.hpp"

#include <thread>

using namespace std::chrono_literals;

void Engine::init()
{
	mFPS = 60;

	Core::Log::init();

	Core::Memory::init();
	Core::Profiler::init();
    EC.init();
    Core::SystemsManager::getInstance().init();

    CREATE_SYSTEM(Core::Time);
    CREATE_SYSTEM(EngineConfig);
    // CREATE_SYSTEM(GPUInterface);
    CREATE_SYSTEM(Core::WindowManager);
    Core::WindowData windowData;
    windowData.mTitle = "Vulkan Engine";
    windowData.mFullScreen = false;
    windowData.mWindowSize.set(800, 600);
    windowData.mMainWindow = true;
    GET_SYSTEM(Core::WindowManager).createWindow(windowData);
    CREATE_SYSTEM(GPUInstance);
    CREATE_SYSTEM(Core::Input);
    GET_SYSTEM(Core::Input).setWindowInputAdapter(GET_SYSTEM(Core::WindowManager).getMainWindow());
    CREATE_SYSTEM(Core::TimerManager);
    CREATE_SYSTEM(Core::EventsManager);
    CREATE_SYSTEM(GPUMeshFactory);
    CREATE_SYSTEM(GPUShaderManager);
    CREATE_SYSTEM(CameraManager);
    CREATE_SYSTEM(GPUSkeletalAnimationManager);
    CREATE_SYSTEM(ModelManager);
    CREATE_SYSTEM(RenderEngine);
    CREATE_SYSTEM(DebugRenderer);
    CREATE_SYSTEM(UIManager);
    CREATE_SYSTEM(ScenesManager);
    CREATE_SYSTEM(Core::CommandLine);
    CREATE_SYSTEM(ScriptEngine);
}

void Engine::preSceneChanged()
{
	GET_SYSTEM(ScriptEngine).preSceneChanged();
	GET_SYSTEM(RenderEngine).preSceneChanged();
	GET_SYSTEM(Core::TimerManager).terminate();
}

void Engine::postSceneChanged()
{
	GET_SYSTEM(RenderEngine).postSceneChanged();
	GET_SYSTEM(ScriptEngine).postSceneChanged();
}

void Engine::run()
{
	f32 inverseFPS = 1.0f / mFPS;
	f32 inverseFPSMillis = inverseFPS * 1000.0f;

	f32 diff = 0;

	while (!GET_SYSTEM(Core::WindowManager).getMainWindow()->isClosed())
	{
        //FrameMarkStart("frame");
		GET_SYSTEM(Core::Time).startFrame();

		if (GET_SYSTEM(ScenesManager).pendingLoadRequests())
		{
			preSceneChanged();
			GET_SYSTEM(ScenesManager).loadPendingScenes();
			postSceneChanged();
		}

		GET_SYSTEM(Core::Input).update();
		GET_SYSTEM(Core::WindowManager).getMainWindow()->pollEvents();

		GET_SYSTEM(Core::CommandLine).update();

		GET_SYSTEM(ScenesManager).update();
		GET_SYSTEM(Core::TimerManager).update();
		GET_SYSTEM(ScriptEngine).update();
		GET_SYSTEM(RenderEngine).update();

		f32 dtMillis = GET_SYSTEM(Core::Time).getElapsedTimeMillis();
		
		if (inverseFPSMillis >= dtMillis)
		{
			diff = inverseFPSMillis - dtMillis;
			auto diff_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double, std::milli>(diff));
			std::this_thread::sleep_for(std::chrono::milliseconds(diff_duration.count()));
		}
		
		GET_SYSTEM(Core::Time).endFrame();
        //FrameMarkEnd("frame");
	}
}

void Engine::terminate()
{

	Core::MemoryTracking::log();
	Core::SystemsManager::getInstance().terminate();
	Core::SystemsManager::deleteInstance();
    EC.terminate();
    EntityComponentManager::deleteInstance();
	Core::Profiler::terminate();
	Core::Memory::terminate();
    Core::HashedStringsManager::terminate();

    LOG("Terminated OK!")

	Core::Log::terminate();
}
