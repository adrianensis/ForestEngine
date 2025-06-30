#include "UISceneTree.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Core/Input/Input.hpp"

void UISceneTree::init()
{
    SceneObject::init();
}

void UISceneTree::update()
{
    // FOR_LIST(it, mTexts)
    // {
    //     GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(it->second);
    // }

    UIBuilder uiBuilder;
	uiBuilder.
	setPosition(Vector2(0,0)).
	// setPosition(startGlyph).
	setAdjustSizeToText(true).
	setIsAffectedByLayout(true).
	setLayout(UILayout::VERTICAL).
    setParent(this).
	setTextScale(0.5f).
	setSize(Vector2(0.5, 0.05f));

    const std::list<TEntityPtr<SceneObject>>& objects = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->getNewSceneObjects();
    std::list<TEntityPtr<SceneObject>> objectsInmutableList;
    FOR_LIST(it, objects)
    {
        objectsInmutableList.push_back(*it);
    }
    FOR_LIST(it, objectsInmutableList)
    {
        Core::HashedString className = Core::ClassManager::getDynamicClassMetadata(&(*it).get()).mClassDefinition.mName;
        Core::HashedString inspectorName(className.get() /*+ std::to_string(id)*/);
        TEntityPtr<UIButton> uiText = uiBuilder.
        setText(inspectorName).
        // setIsStatic(false).
        create<UIButton>().
        getUIElement<UIButton>();

        // mTexts.emplace(id, uiText);
    }
}

void UISceneTree::onDestroy()
{

    SceneObject::onDestroy();
}