#include "UI/UIGroup.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "Scene/Scene.hpp"


void UIGroup::init()
{
	mName = "";
}

void UIGroup::addUIElement(EC::TEntityPtr<UIElement> uiElement)
{
	if (uiElement)
	{
		mUIElements.push_back(uiElement);
	}
}

void UIGroup::removeUIElement(EC::TEntityPtr<UIElement> uiElement)
{
	if (uiElement)
	{
		mUIElements.remove(uiElement);
	}
}

void UIGroup::destroyUIElement(EC::TEntityPtr<UIElement> uiElement)
{
	if (uiElement)
	{
		removeUIElement(uiElement);
		uiElement->mScene->removeSceneObject(uiElement);
	}
}

void UIGroup::destroyAllUIElements()
{
	FOR_LIST(it, mUIElements)
	{
		EC::TEntityPtr<UIElement> element = *it;
		element->mScene->removeSceneObject(element);
	}

	mUIElements.clear();
}

void UIGroup::setVisibility(bool visibility)
{
	mVisible = visibility;

	FOR_LIST(it, mUIElements)
	{
		(*it)->setVisibility(mVisible);
	}
}
