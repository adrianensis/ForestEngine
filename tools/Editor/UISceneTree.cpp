#include "UISceneTree.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Core/Input/Input.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"

void UISceneTree::init()
{
    GameObject::init();
}

void UISceneTree::update()
{
    FOR_LIST(it, mTexts)
    {
        GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(it->second);
    }

    UIBuilder uiBuilder;
	uiBuilder.
	setPosition(Vector2(0,0)).
	// setPosition(startGlyph).
	setAdjustSizeToText(true).
	setIsAffectedByLayout(true).
	setLayout(UILayout::VERTICAL).
    setParent(getPtrToThis<GameObject>()).
	setTextScale(0.5f).
	setSize(Vector2(0.5, 0.05f));

    const std::list<Ptr<GameObject>>& objects = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->getNewGameObjects();
    std::list<Ptr<GameObject>> objectsInmutableList;
    FOR_LIST(it, objects)
    {
        objectsInmutableList.push_back(*it);
    }
    FOR_LIST(it, objectsInmutableList)
    {
        HashedString className = ClassManager::getDynamicClassMetadata(&(*it).get()).mClassDefinition.mName;
        ObjectId id = (*it)->getObjectId();
        HashedString inspectorName(className.get() + std::to_string(id));
        Ptr<UIButton> uiText = uiBuilder.
        setText(inspectorName).
        // setIsStatic(false).
        create<UIButton>().
        getUIElement<UIButton>();

        mTexts.emplace(id, uiText);
    }
}

void UISceneTree::onDestroy()
{

    GameObject::onDestroy();
}