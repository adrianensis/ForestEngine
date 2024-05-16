#include "Graphics/RenderEngine.hpp"
#include "Graphics/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/RenderSharedContext.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Scene/Module.hpp"
#include "Engine/EngineConfig.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"

void RenderEngine::init()
{
	LOG_TRACE()

	registerComponentClass(ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId());
	registerComponentClass(ClassManager::getClassMetadata<Light>().mClassDefinition.getId());

	mCameraDirtyTranslation = true;

    mRenderPipeline.init();

	// octree.init(5000);
}

void RenderEngine::update()
{
	PROFILER_CPU()

	if (mRenderPipelineData.mCamera)
	{
		mRenderPipelineData.mCamera->update();
	}

    std::vector<Ptr<MeshRenderer>> newList;
    FOR_ARRAY(i, mRenderers)
    {
        PROFILER_BLOCK_CPU("remove renderers");

        Ptr<MeshRenderer> renderer = mRenderers[i];
        if(renderer.isValid())
        {
            newList.push_back(renderer);
        }
    }

    mRenderers.clear();
    mRenderers = newList;

    FOR_ARRAY(i, mRenderers)
    {
        PROFILER_BLOCK_CPU("update renderers");
        Ptr<MeshRenderer> renderer = mRenderers[i];
        renderer->update();
        const Matrix4& rendererModelMatrix = renderer->getRendererModelMatrix();
        GET_SYSTEM(RenderSharedContext).setInstanceMatrix(renderer->getRenderInstanceSlot(), rendererModelMatrix);
        GET_SYSTEM(RenderSharedContext).setMaterialInstanceProperties(renderer->getMaterialInstanceSlot(), renderer->getMaterialInstance());
    }

    GET_SYSTEM(RenderSharedContext).update();
    
	GET_SYSTEM(AnimationManager).update();

	// octree.update();

	render();
	swap();
}

void RenderEngine::preSceneChanged()
{
	LOG_TRACE()
}

void RenderEngine::postSceneChanged()
{
	LOG_TRACE()
    mRenderPipelineData.mCamera = GET_SYSTEM(ScenesManager).getCurrentCamera();
}

void RenderEngine::onResize(u32 width, u32 height)
{
	LOG_TRACE()
	GET_SYSTEM(GPUInterface).setViewport(0, 0, width, height);
    mRenderPipelineData.mCamera->onResize();
}

void RenderEngine::terminate()
{
	LOG_TRACE()
    
    mRenderPipeline.terminate();
}

void RenderEngine::addComponent(Ptr<SystemComponent> component)
{
	System::addComponent(component);

    if(component->getSystemComponentId() == ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId())
    {
        Ptr<MeshRenderer> renderer = Ptr<MeshRenderer>::cast(component);
        mRenderers.push_back(renderer);

        mRenderPipeline.addRenderer(renderer);

        if(renderer->getGeometricSpace() == GeometricSpace::WORLD)
        {
            //octree.addOcTreeElement(Ptr<IOcTreeElement>::cast(renderer));
        }
    }
    else if(component->getSystemComponentId() == ClassManager::getClassMetadata<Light>().mClassDefinition.getId())
    {
        if(ClassManager::getDynamicClassMetadata(component.getInternalPointer()).mClassDefinition.getId() == ClassManager::getClassMetadata<PointLight>().mClassDefinition.getId())
        {
            mRenderPipelineData.mPointLights.push_back(Ptr<PointLight>::cast(component));
        }
        else if(ClassManager::getDynamicClassMetadata(component.getInternalPointer()).mClassDefinition.getId() == ClassManager::getClassMetadata<DirectionalLight>().mClassDefinition.getId())
        {
            mRenderPipelineData.mDirectionalLight = Ptr<DirectionalLight>::cast(component);
        }
    }
}

void RenderEngine::removeComponent(Ptr<SystemComponent> component)
{
	System::removeComponent(component);

    if(component->getSystemComponentId() == ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId())
    {
        Ptr<MeshRenderer> renderer = Ptr<MeshRenderer>::cast(component);
        mRenderPipeline.removeRenderer(renderer);
    }
    else if(component->getSystemComponentId() == ClassManager::getClassMetadata<Light>().mClassDefinition.getId())
    {
    }
}

void RenderEngine::swap()
{
	PROFILER_CPU()

	GET_SYSTEM(Window).swap();
}

void RenderEngine::render()
{
	PROFILER_CPU()

    mRenderPipeline.render(mRenderPipelineData);
}