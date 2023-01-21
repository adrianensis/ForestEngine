#include "UI/UIGroup.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "Scene/Scene.hpp"


void UIGroup::init()
{
	mName = "";
}

void UIGroup::addUIElement(UIElement *uiElement)
{
	if (uiElement)
	{
		mUIElements.push_back(uiElement);
	}
}

void UIGroup::removeUIElement(UIElement *uiElement)
{
	if (uiElement)
	{
		mUIElements.remove(uiElement);
	}
}

void UIGroup::destroyUIElement(UIElement *uiElement)
{
	if (uiElement)
	{
		removeUIElement(uiElement);
		uiElement->mScene->removeGameObject(uiElement);
	}
}

void UIGroup::destroyAllUIElements()
{
	FOR_LIST(it, mUIElements)
	{
		UIElement *element = *it;
		element->mScene->removeGameObject(element);
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
