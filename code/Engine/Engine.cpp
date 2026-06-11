#include "Engine/Engine.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Engine/EngineConfig.hpp"
#include "CommandLine/CommandLine.hpp"
#include "Engine/Paths.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Input/Input.hpp"
#include "Core/Event/EventsManager.hpp"
#include "Core/Time/TimeUtils.hpp"
#include "Core/Time/TimerManager.hpp"

#include "Core/System/SystemsManager.hpp"
#include "Window/WindowManager.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"

#include "Scripting/ScriptEngine.hpp"

#include "Scene/ScenesManager.hpp"
#include "UI/UIManager.hpp"

using namespace std::chrono_literals;

void Engine::init()
{
	mFPS = 60;

	Core::Log::init(Paths::PredefinedPaths::mOutput.get());

	Core::Memory::init();
	Core::Profiler::init();

	System::SystemsManager::getInstance().init();
	
	CREATE_SYSTEM(Time::Time);
    GET_SYSTEM(Time::Time).init();
    CREATE_SYSTEM(Time::TimerManager);
    GET_SYSTEM(Time::TimerManager).init();
    CREATE_SYSTEM(Event::EventsManager);
    GET_SYSTEM(Event::EventsManager).init();
    CREATE_SYSTEM(EC::EntityComponentManager);
    GET_SYSTEM(EC::EntityComponentManager).init();

    CREATE_SYSTEM(EngineConfig);
    GET_SYSTEM(EngineConfig).init();
    CREATE_SYSTEM(Window::WindowManager);
    GET_SYSTEM(Window::WindowManager).init();
    Window::WindowData windowData;
    windowData.mTitle = "Vulkan Engine";
    windowData.mFullScreen = false;
    windowData.mWindowSize = {1080, 720};
    windowData.mMainWindow = true;
    Core::WeakPtr<Window::Window> window = GET_SYSTEM(Window::WindowManager).createWindow(windowData);

	// TODO: GPUInstance should be propagated, not singleton
    GPUInstance::getInstance().init(window.getInternalPointer());

    CREATE_SYSTEM(Input::Input);
    GET_SYSTEM(Input::Input).init();
    GET_SYSTEM(Input::Input).setWindowInputAdapter(GET_SYSTEM(Window::WindowManager).getMainWindow().getInternalPointer());
    CREATE_SYSTEM(CameraManager);
	
    CREATE_SYSTEM(RenderEngine);
    GET_SYSTEM(RenderEngine).init();
	GET_SYSTEM(Window::WindowManager).getMainWindow()->addWindowListener(GET_SYSTEM_PTR(RenderEngine));

	GET_SYSTEM(EC::EntityComponentManager).addComponentListener<MeshRenderer>(GET_SYSTEM_PTR(RenderEngine));
	GET_SYSTEM(EC::EntityComponentManager).addComponentListener<Light>(GET_SYSTEM_PTR(RenderEngine));
    CREATE_SYSTEM(DebugRenderer);
    GET_SYSTEM(DebugRenderer).init();
    CREATE_SYSTEM(ModelManager);
    GET_SYSTEM(ModelManager).init();

    CREATE_SYSTEM(UIManager);
    GET_SYSTEM(UIManager).init();

    CREATE_SYSTEM(ScenesManager);
    GET_SYSTEM(ScenesManager).init();

    CREATE_SYSTEM(Command::CommandLine);
    GET_SYSTEM(Command::CommandLine).init();
    CREATE_SYSTEM(ScriptEngine);
    GET_SYSTEM(ScriptEngine).init();
	GET_SYSTEM(EC::EntityComponentManager).addComponentListener<Script>(GET_SYSTEM_PTR(ScriptEngine));
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

		GET_SYSTEM(CameraManager).update();
		GET_SYSTEM(ScenesManager).update();
		GET_SYSTEM(Time::TimerManager).update(GET_SYSTEM(Time::Time).getDeltaTimeMillis());
		GET_SYSTEM(ScriptEngine).update(GET_SYSTEM(Time::Time).getDeltaTimeMillis());
		GET_SYSTEM(RenderEngine).update(GET_SYSTEM(Time::Time).getDeltaTimeMillis());

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
	GET_SYSTEM(EC::EntityComponentManager).removeAllListeners();
	System::SystemsManager::getInstance().terminate();
	System::SystemsManager::deleteInstance();
	GPUInstance::getInstance().terminate();
	GPUInstance::deleteInstance();
	
	Core::Profiler::terminate();
	Core::Memory::terminate();
    Core::HashedStringsManager::terminate();

    LOG("Terminated OK!")

	Core::Log::terminate();
}
