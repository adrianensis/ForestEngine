#include "UI/UIGroup.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "Scene/Scene.hpp"


void UIGroup::init()
{
	mName = "";
}

void UIGroup::addUIElement(Ptr<UIElement> uiElement)
{
	if (uiElement)
	{
		mUIElements.push_back(uiElement);
	}
}

void UIGroup::removeUIElement(Ptr<UIElement> uiElement)
{
	if (uiElement)
	{
		mUIElements.remove(uiElement);
	}
}

void UIGroup::destroyUIElement(Ptr<UIElement> uiElement)
{
	if (uiElement)
	{
		removeUIElement(uiElement);
		uiElement.get().mScene.get().removeGameObject(Ptr<GameObject>::cast(uiElement));
	}
}

void UIGroup::destroyAllUIElements()
{
	FOR_LIST(it, mUIElements)
	{
		Ptr<UIElement> element = *it;
		element.get().mScene.get().removeGameObject(Ptr<GameObject>::cast(element));
	}

	mUIElements.clear();
}

void UIGroup::setVisibility(bool visibility)
{
	mVisible = visibility;

	FOR_LIST(it, mUIElements)
	{
		(*it).get().setVisibility(mVisible);
	}
}
