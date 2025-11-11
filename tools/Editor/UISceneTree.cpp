#include "UISceneTree.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Engine/Input/Input.hpp"

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
    setParent(ECManager.getEntityPtr(this)).
	setSize(Maths::Vector2(0.5, 0.05f));

    const std::list<EC::EntityPtr<GameObject>>& objects = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->getNewGameObjects();
    std::list<EC::EntityPtr<GameObject>> objectsInmutableList;
    FOR_LIST(it, objects)
    {
        objectsInmutableList.push_back(*it);
    }
    FOR_LIST(it, objectsInmutableList)
    {
        Core::HashedString className = Core::ClassManager::getDynamicClassMetadata(&(*it).get()).mClassDefinition.mName;
        Core::HashedString inspectorName(className.get() /*+ std::to_string(id)*/);
        EC::EntityPtr<UIButton> uiText = uiBuilder.
        setText(inspectorName).
        // setIsStatic(false).
        create<UIButton>().
        getUIElement<UIButton>();

        // mTexts.emplace(id, uiText);
    }
}

void UISceneTree::onDestroy()
{

    GameObject::onDestroy();
}