#include "Core/Engine.hpp"
#include "Core/EngineConfig.hpp"
#include "Graphics/Module.hpp"
#include "Scripting/Module.hpp"

#include "Scene/Module.hpp"
#include "UI/Module.hpp"

using namespace std::chrono_literals;

void Engine::init()
{
	mFPS = 60;

	Memory::init();

	ClassManager::getInstance().init();

	Profiler::getInstance().init();

	EngineConfig::getInstance().init();
	RenderContext::init();
	Input::getInstance().init();
	TimerManager::getInstance().init();
	EventsManager::getInstance().init();
	MeshPrimitives::getInstance().init();
	MaterialManager::getInstance().init();
	UIManager::getInstance().init();
	ScenesManager::getInstance().init();

	CommandLine::getInstance().init();
}

void Engine::initEngineSystems()
{
	f32 sceneSize = ScenesManager::getInstance().getCurrentScene()->getSize();
	RenderEngine::getInstance().init(sceneSize);
	ScriptEngine::getInstance().init();
}

void Engine::terminateSubSystems()
{
	ScriptEngine::getInstance().terminate();
	RenderEngine::getInstance().terminate();

	TimerManager::getInstance().terminate();
}

void Engine::run()
{
	Time::getInstance().init();

	f32 inverseFPS = 1.0f / mFPS;
	f32 inverseFPSMillis = inverseFPS * 1000.0f;

	f32 diff = 0;

	while (!RenderContext::isClosed())
	{
		Time::getInstance().startFrame();

		if (ScenesManager::getInstance().getSceneHasChanged())
		{
			terminateSubSystems();
			ScenesManager::getInstance().loadCurrentScene();
			initEngineSystems();
		}

		Input::getInstance().pollEvents();

		CommandLine::getInstance().update();

		ScenesManager::getInstance().update();
		TimerManager::getInstance().update();
		ScriptEngine::getInstance().update();
		RenderEngine::getInstance().update();

		f32 dtMillis = Time::getInstance().getElapsedTimeMillis();
		
		if (inverseFPSMillis > dtMillis)
		{
			diff = inverseFPSMillis - dtMillis;
			auto diff_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double, std::milli>(diff));
			std::this_thread::sleep_for(std::chrono::milliseconds(diff_duration.count()));
		}
		
		std::cout << std::round(1.0f/Time::getInstance().getDeltaTimeSeconds()) << std::endl;

		Profiler::getInstance().update(Time::getInstance().getElapsedTimeMillis());

		Time::getInstance().endFrame();
	}
}

void Engine::terminate()
{
	TRACE();
	
	CommandLine::getInstance().terminate();
	CommandLine::deleteInstance();

	ScenesManager::deleteInstance();

	terminateSubSystems();

	UIManager::getInstance().terminate();
	UIManager::deleteInstance();

	MaterialManager::deleteInstance();

	MeshPrimitives::getInstance().terminate();
	MeshPrimitives::getInstance().deleteInstance();

	ScriptEngine::deleteInstance();
	RenderEngine::deleteInstance();
	EventsManager::getInstance().terminate();
	EventsManager::deleteInstance();
	TimerManager::deleteInstance();
	Time::deleteInstance();
	Input::deleteInstance();
	RenderContext::terminate();

	EngineConfig::deleteInstance();

	EngineSystemsManager::deleteInstance();

	Profiler::getInstance().terminate();

	ClassManager::deleteInstance();

	Memory::terminate();
}