#include "Graphics/RenderEngine.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "GPU/GPUInstance.hpp"
#include "Graphics/Light/Light.hpp"
#include "Window/WindowManager.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/Module.hpp"
#include "Engine/EngineConfig.hpp"

void RenderEngine::init()
{
    mRenderPipeline = Core::OwnerPtr<RenderPipelinePBR>::newObject();
    mRenderPipeline->init();
    mRenderPipeline->compile();

	// octree.init(5000);

    GET_SYSTEM(Window::WindowManager).getMainWindow()->addWindowListener(this);

    // TODO: put in a better place?
    // Set default ambient light.
    mRenderPipelineUpdateData.mAmbientLightData.mDiffuse.set(0.1,0.1,0.1, 0);
}

void RenderEngine::update(Core::f32 dt)
{
    PROFILER_CPU()
    mRenderPipeline->update(dt, mRenderPipelineUpdateData);
    mRenderPipeline->render();

    GPUInstance::getInstance().mGPUContext->currentFrame = (GPUInstance::getInstance().mGPUContext->currentFrame + 1) % GPUContext::MAX_FRAMES_IN_FLIGHT;
}

void RenderEngine::preSceneChanged()
{
    mCompileRequest = true;
}

void RenderEngine::postSceneChanged()
{
}

void RenderEngine::onResize()
{
//	GET_SYSTEM(GPUInterface).setViewport(0, 0, width, height);
    GPUInstance::getInstance().mGPUContext->setWindowResized();
    GPUInstance::getInstance().mGPUContext->recreateRenderingObjects();
    mRenderPipeline->onResize();
    Camera* camera = GET_SYSTEM(CameraManager).getCamera();
    camera->onResize();
}

void RenderEngine::terminate()
{
    
    mRenderPipeline->terminate();
}

void RenderEngine::onComponentAdded(EC::Component* component)
{
    const Core::ClassMetadata& classMetadata = Core::ClassManager::getDynamicClassMetadata(component);
    if(classMetadata.mClassDefinition.isA(Core::ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId()))
    {
        MeshRenderer* renderer = CAST(MeshRenderer, component);
        mRenderPipeline->addRenderer(renderer);

        // if(renderer->getGeometricSpace() == Maths::GeometricSpace::WORLD)
        // {
        //     //octree.addOcTreeElement(Core::WeakPtr<IOcTreeElement>::cast(renderer));
        // }
    }
    else if(classMetadata.mClassDefinition.isA(Core::ClassManager::getClassMetadata<Light>().mClassDefinition.getId()))
    {
        if(classMetadata.mClassDefinition.isA(Core::ClassManager::getClassMetadata<PointLight>().mClassDefinition.getId()))
        {
            PointLight* pointLight = CAST(PointLight, component);
            mRenderPipelineUpdateData.mPointLightsData.push_back(pointLight->calculateLightData());
        }
        else if(classMetadata.mClassDefinition.isA(Core::ClassManager::getClassMetadata<DirectionalLight>().mClassDefinition.getId()))
        {
            DirectionalLight* directionalLight = CAST(DirectionalLight, component);
            mRenderPipelineUpdateData.mDirectionalLightData = directionalLight->calculateLightData();
        }
    }
}

void RenderEngine::onComponentRemoved(EC::Component* component)
{
    const Core::ClassMetadata& classMetadata = Core::ClassManager::getDynamicClassMetadata(component);
    if(classMetadata.mClassDefinition.isA(Core::ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId()))
    {
        MeshRenderer* renderer = CAST(MeshRenderer, component);
        mRenderPipeline->removeRenderer(renderer);
    }
    else if(classMetadata.mClassDefinition.isA(Core::ClassManager::getClassMetadata<Light>().mClassDefinition.getId()))
    {
    }
}