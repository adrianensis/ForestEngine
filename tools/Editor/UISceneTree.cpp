#include "UISceneTree.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Input/Input.hpp"

void UISceneTree::init()
{
    GameObject::init();
}

void UISceneTree::update()
{
    // FOR_LIST(it, mTexts)
    // {
    //     GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(it->second);
    // }

    UIBuilder uiBuilder;
	uiBuilder.
	setPosition(Maths::Vector2(0,0)).
	// setPosition(startGlyph).
	setAdjustSizeToText(true).
	setIsAffectedByLayout(true).
	setLayout(UILayout::VERTICAL).
    setParent(this).
	setSize(Maths::Vector2(0.5, 0.05f));

    const std::list<GameObject*>& objects = mScene->getScenesManager()->getScene(ScenesManager::smDefaultSceneName)->getNewGameObjects();
    std::list<GameObject*> objectsInmutableList;
    FOR_LIST(it, objects)
    {
        objectsInmutableList.push_back(*it);
    }
    FOR_LIST(it, objectsInmutableList)
    {
        Core::HashedString className = Core::ClassManager::getDynamicClassMetadata((*it)).mClassDefinition.mName;
        Core::HashedString inspectorName(className.get() /*+ std::to_string(id)*/);
        UIButton* uiText = uiBuilder.
        setText(inspectorName).
        // setIsStatic(false).
        create<UIButton>(mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName).getInternalPointer()).
        getUIElement<UIButton>();

        // mTexts.emplace(id, uiText);
    }
}

void UISceneTree::onDestroy()
{

    GameObject::onDestroy();
}