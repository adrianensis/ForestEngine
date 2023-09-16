#include "Engine/Engine.hpp"
#include "Engine/EngineConfig.hpp"
#include "Graphics/Module.hpp"
#include "Scripting/Module.hpp"

#include "Scene/Module.hpp"
#include "UI/Module.hpp"

using namespace std::chrono_literals;

void Engine::init()
{
	mFPS = 500;

	Memory::init();

    CREATE_SYSTEM(Profiler);
	GET_SYSTEM(Profiler).init();

    CREATE_SYSTEM(EngineConfig);
	GET_SYSTEM(EngineConfig).init();
    CREATE_SYSTEM(GPUInterface);
    CREATE_SYSTEM(RenderContext);
	GET_SYSTEM(RenderContext).init();
    CREATE_SYSTEM(Input);
	GET_SYSTEM(Input).init();
    CREATE_SYSTEM(TimerManager);
	GET_SYSTEM(TimerManager).init();
    CREATE_SYSTEM(EventsManager);
	GET_SYSTEM(EventsManager).init();
    CREATE_SYSTEM(MeshPrimitives);
	GET_SYSTEM(MeshPrimitives).init();
    CREATE_SYSTEM(MaterialManager);
	GET_SYSTEM(MaterialManager).init();
    CREATE_SYSTEM(AnimationManager);
	GET_SYSTEM(AnimationManager).init();
    CREATE_SYSTEM(ModelManager);
	GET_SYSTEM(ModelManager).init();
    CREATE_SYSTEM(UIManager);
	GET_SYSTEM(UIManager).init();
    CREATE_SYSTEM(ScenesManager);
	GET_SYSTEM(ScenesManager).init();

    CREATE_SYSTEM(CommandLine);
	GET_SYSTEM(CommandLine).init();

    CREATE_SYSTEM(RenderEngine);
    CREATE_SYSTEM(ScriptEngine);

    //REGISTER_ENGINE_SYSTEM(Ptr<EngineSystem>::cast(RenderEngine::getInstancePtr()));
}

void Engine::initEngineSystems()
{
	f32 sceneSize = GET_SYSTEM(ScenesManager).getCurrentScene()->getSize();
	GET_SYSTEM(RenderEngine).init(sceneSize);
	GET_SYSTEM(ScriptEngine).init();
}

void Engine::terminateSubSystems()
{
	GET_SYSTEM(ScriptEngine).terminate();
	GET_SYSTEM(RenderEngine).terminate();
    //GET_SYSTEM(RenderEngine).terminate();

	GET_SYSTEM(TimerManager).terminate();
}

void Engine::run()
{
    CREATE_SYSTEM(Time);
	GET_SYSTEM(Time).init();

	f32 inverseFPS = 1.0f / mFPS;
	f32 inverseFPSMillis = inverseFPS * 1000.0f;

	f32 diff = 0;

	while (!GET_SYSTEM(RenderContext).isClosed())
	{
		GET_SYSTEM(Time).startFrame();

		if (GET_SYSTEM(ScenesManager).getSceneHasChanged())
		{
			terminateSubSystems();
			GET_SYSTEM(ScenesManager).loadCurrentScene();
			initEngineSystems();
		}

		GET_SYSTEM(Input).pollEvents();

		GET_SYSTEM(CommandLine).update();

		GET_SYSTEM(ScenesManager).update();
		GET_SYSTEM(TimerManager).update();
		GET_SYSTEM(ScriptEngine).update();
		GET_SYSTEM(RenderEngine).update();

		f32 dtMillis = GET_SYSTEM(Time).getElapsedTimeMillis();
		
		if (inverseFPSMillis >= dtMillis)
		{
			diff = inverseFPSMillis - dtMillis;
			auto diff_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double, std::milli>(diff));
			std::this_thread::sleep_for(std::chrono::milliseconds(diff_duration.count()));
		}
		
		GET_SYSTEM(Time).endFrame();
        f32 fps = 1000.0f/GET_SYSTEM(Time).getDeltaTimeMillis();
		VAR(fps)
	}
}

void Engine::terminate()
{
	TRACE();
	
	GET_SYSTEM(CommandLine).terminate();
    REMOVE_SYSTEM(CommandLine);

    REMOVE_SYSTEM(ScenesManager);

	terminateSubSystems();

	GET_SYSTEM(UIManager).terminate();
    REMOVE_SYSTEM(UIManager);

	GET_SYSTEM(ModelManager).terminate();
    REMOVE_SYSTEM(ModelManager);

    REMOVE_SYSTEM(MaterialManager);

	GET_SYSTEM(AnimationManager).terminate();
    REMOVE_SYSTEM(AnimationManager);

	GET_SYSTEM(MeshPrimitives).terminate();
    REMOVE_SYSTEM(MeshPrimitives);

    REMOVE_SYSTEM(ScriptEngine);
    REMOVE_SYSTEM(RenderEngine);
	GET_SYSTEM(EventsManager).terminate();
    REMOVE_SYSTEM(EventsManager);
    REMOVE_SYSTEM(TimerManager);
    REMOVE_SYSTEM(Time);
    REMOVE_SYSTEM(Input);
    REMOVE_SYSTEM(EngineConfig);
	GET_SYSTEM(RenderContext).terminate();
    REMOVE_SYSTEM(RenderContext);
    REMOVE_SYSTEM(GPUInterface);

	GET_SYSTEM(Profiler).terminate();
	REMOVE_SYSTEM(Profiler);

	EngineSystemsManager::deleteInstance();

	Memory::terminate();
}
