#include "UISceneTree.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Engine/Input/Input.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"

void UISceneTree::init()
{
    GameObject::init();

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

    const std::list<OwnerPtr<GameObject>>& objects = GET_SYSTEM(ScenesManager).getCurrentScene()->getNewGameObjects();
    std::list<Ptr<GameObject>> objectsInmutableList;
    FOR_LIST(it, objects)
    {
        objectsInmutableList.push_back(*it);
    }
    FOR_LIST(it, objectsInmutableList)
    {
        Ptr<UIText> uiText = uiBuilder.
        setText(ClassManager::getDynamicClassMetadata(&(*it).get()).mClassDefinition.mName).
        // setIsStatic(false).
        create<UIText>().
        getUIElement<UIText>();

        mTexts.emplace((*it)->getObjectId(), uiText);
    }
}

void UISceneTree::update()
{

}

void UISceneTree::onDestroy()
{

    GameObject::onDestroy();
}