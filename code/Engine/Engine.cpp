#include "Engine/Engine.hpp"
#include "Engine/EngineConfig.hpp"
#include "Engine/Command/CommandLine.hpp"
#include "Engine/Time/TimerManager.hpp"
#include "Engine/Input/Input.hpp"
#include "Engine/Events/EventsManager.hpp"
#include "Graphics/Module.hpp"
#include "Scripting/Module.hpp"

#include "Scene/Module.hpp"
#include "UI/Module.hpp"

#include <thread>

using namespace std::chrono_literals;

void Engine::init()
{
	mFPS = 60;

	Memory::init();
	Profiler::init();

    CREATE_SYSTEM(EngineConfig);
    CREATE_SYSTEM(GPUInterface);
    CREATE_SYSTEM(WindowManager);
    WindowData windowData;
    windowData.mTitle = "Engine";
    windowData.mFullScreen = true;
    windowData.mMainWindow = true;
    GET_SYSTEM(WindowManager).createWindow(windowData);
    CREATE_SYSTEM(GPUGlobalState);
    CREATE_SYSTEM(Input);
    CREATE_SYSTEM(TimerManager);
    CREATE_SYSTEM(EventsManager);
    CREATE_SYSTEM(ComponentsManager);
    CREATE_SYSTEM(MeshPrimitives);
    CREATE_SYSTEM(MaterialManager);
    CREATE_SYSTEM(CameraManager);
    CREATE_SYSTEM(SkeletalAnimationManager);
    CREATE_SYSTEM(ModelManager);
    CREATE_SYSTEM(RenderEngine);
    CREATE_SYSTEM(DebugRenderer);
    CREATE_SYSTEM(UIManager);
    CREATE_SYSTEM(ScenesManager);
    CREATE_SYSTEM(CommandLine);
    CREATE_SYSTEM(ScriptEngine);
}

void Engine::preSceneChanged()
{
	GET_SYSTEM(ScriptEngine).preSceneChanged();
	GET_SYSTEM(RenderEngine).preSceneChanged();
	GET_SYSTEM(TimerManager).terminate();
}

void Engine::postSceneChanged()
{
	GET_SYSTEM(RenderEngine).postSceneChanged();
	GET_SYSTEM(ScriptEngine).postSceneChanged();
}

void Engine::run()
{
    CREATE_SYSTEM(Time);

	f32 inverseFPS = 1.0f / mFPS;
	f32 inverseFPSMillis = inverseFPS * 1000.0f;

	f32 diff = 0;

	while (!GET_SYSTEM(WindowManager).getMainWindow()->isClosed())
	{
		GET_SYSTEM(Time).startFrame();

		if (GET_SYSTEM(ScenesManager).pendingLoadRequests())
		{
			preSceneChanged();
			GET_SYSTEM(ScenesManager).loadPendingScenes();
			postSceneChanged();
		}

		GET_SYSTEM(Input).update();
		GET_SYSTEM(WindowManager).getMainWindow()->pollEvents();

		GET_SYSTEM(CommandLine).update();

		GET_SYSTEM(ScenesManager).update();
		GET_SYSTEM(TimerManager).update();
		GET_SYSTEM(ScriptEngine).update();
		GET_SYSTEM(RenderEngine).update();

		f32 dtMillis = GET_SYSTEM(Time).getElapsedTimeMillis();
		
		// if (inverseFPSMillis >= dtMillis)
		// {
		// 	diff = inverseFPSMillis - dtMillis;
		// 	auto diff_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double, std::milli>(diff));
		// 	std::this_thread::sleep_for(std::chrono::milliseconds(diff_duration.count()));
		// }
		
		GET_SYSTEM(Time).endFrame();
	}
}

void Engine::terminate()
{
	LOG_TRACE();

	SystemsManager::getInstance().terminate();
	Profiler::terminate();
	Memory::terminate();
}
