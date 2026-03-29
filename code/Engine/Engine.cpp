#include "Engine/Engine.hpp"
#include "Engine/EngineConfig.hpp"
#include "CommandLine/CommandLine.hpp"
#include "Engine/Paths.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Input/Input.hpp"
#include "Core/Event/EventsManager.hpp"

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
    mTime.init();
    mTimerManager.init();
	Event::EventsManager::getInstance().init();
    ECManager.init();
    System::SystemsManager::getInstance().init();

    CREATE_SYSTEM(EngineConfig);
    GET_SYSTEM(EngineConfig).init();
    CREATE_SYSTEM(Window::WindowManager);
    GET_SYSTEM(Window::WindowManager).init();
    Window::WindowData windowData;
    windowData.mTitle = "Vulkan Engine";
    windowData.mFullScreen = false;
    windowData.mWindowSize.set(1080, 720);
    windowData.mMainWindow = true;
    Core::WeakPtr<Window::Window> window = GET_SYSTEM(Window::WindowManager).createWindow(windowData);
    GPUInstance::getInstance().init(window.getInternalPointer());
    CREATE_SYSTEM(Input::Input);
    GET_SYSTEM(Input::Input).init();
    CREATE_SYSTEM(CameraManager);
    GET_SYSTEM(Input::Input).setWindowInputAdapter(GET_SYSTEM(Window::WindowManager).getMainWindow());
    CREATE_SYSTEM(RenderEngine);
    GET_SYSTEM(RenderEngine).init(GET_SYSTEM_PTR(CameraManager).getInternalPointer());
	GET_SYSTEM(Window::WindowManager).getMainWindow()->addWindowListener(GET_SYSTEM_PTR(RenderEngine).getInternalPointer());
	ECManager.addComponentListener<MeshRenderer>(GET_SYSTEM_PTR(RenderEngine));
	ECManager.addComponentListener<Light>(GET_SYSTEM_PTR(RenderEngine));
    CREATE_SYSTEM(DebugRenderer);
    GET_SYSTEM(DebugRenderer).init();
    CREATE_SYSTEM(ModelManager);
    GET_SYSTEM(ModelManager).init();
    CREATE_SYSTEM(UIManager);
    GET_SYSTEM(UIManager).init();
    CREATE_SYSTEM(ScenesManager);
    mScenesManager = GET_SYSTEM_PTR(ScenesManager).getInternalPointer();
    GET_SYSTEM(ScenesManager).init(ScenesManagerData
		{
			GET_SYSTEM_PTR(CameraManager).getInternalPointer(), GET_SYSTEM(Window::WindowManager).getMainWindow()->getAspectRatio()
		});
    CREATE_SYSTEM(Command::CommandLine);
    GET_SYSTEM(Command::CommandLine).init();
    CREATE_SYSTEM(ScriptEngine);
    GET_SYSTEM(ScriptEngine).init();
	ECManager.addComponentListener<Script>(GET_SYSTEM_PTR(ScriptEngine));
}

void Engine::preSceneChanged()
{
	GET_SYSTEM(ScriptEngine).preSceneChanged();
	GET_SYSTEM(RenderEngine).preSceneChanged();
	mTimerManager.terminate();
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
		mTime.startFrame();

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
		mTimerManager.update(mTime.getDeltaTimeMillis());
		GET_SYSTEM(ScriptEngine).update(mTime.getDeltaTimeMillis());
		GET_SYSTEM(RenderEngine).update(mTime.getDeltaTimeMillis());

		Core::f32 dtMillis = mTime.getElapsedTimeMillis();
		
		if (inverseFPSMillis >= dtMillis)
		{
			diff = inverseFPSMillis - dtMillis;
			auto diff_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double, std::milli>(diff));
			std::this_thread::sleep_for(std::chrono::milliseconds(diff_duration.count()));
		}
		
		mTime.endFrame();
        //FrameMarkEnd("frame");
	}
}

void Engine::terminate()
{
	Core::MemoryTracking::log();
	System::SystemsManager::getInstance().terminate();
	System::SystemsManager::deleteInstance();
	GPUInstance::getInstance().terminate();
	GPUInstance::deleteInstance();
	ECManager.terminate();
    EC::EntityComponentManager::deleteInstance();
	Event::EventsManager::getInstance().terminate();
	Event::EventsManager::deleteInstance();
	mTimerManager.terminate();
	
	Core::Profiler::terminate();
	Core::Memory::terminate();
    Core::HashedStringsManager::terminate();

    LOG("Terminated OK!")

	Core::Log::terminate();
}
