#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIGroup.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "Scene/Scene.hpp"
#endif

class UIElement;

class UIGroup: public ObjectBase
{
    GENERATE_METADATA(UIGroup)

public:
	CPP void init()
	{
		mName = "";
	}

	CPP void addUIElement(UIElement *uiElement)
	{
		if (uiElement)
		{
			mUIElements.push_back(uiElement);
		}
	}

	CPP void removeUIElement(UIElement *uiElement)
	{
		if (uiElement)
		{
			mUIElements.remove(uiElement);
		}
	}

	CPP void destroyUIElement(UIElement *uiElement)
	{
		if (uiElement)
		{
			removeUIElement(uiElement);
			uiElement->getScene()->removeGameObject(uiElement);
		}
	}

	CPP void destroyAllUIElements()
	{
		FOR_LIST(it, mUIElements)
		{
			UIElement *element = *it;
			element->getScene()->removeGameObject(element);
		}

		mUIElements.clear();
	}

	CPP void setVisibility(bool visibility)
	{
		mVisible = visibility;

		FOR_LIST(it, mUIElements)
		{
			(*it)->setVisibility(mVisible);
		}
	}

public:
	std::string mName;
	bool mVisible = false;
	std::list<UIElement *> mUIElements;
	
	CRGET(Name)
	GET(Visible)
	CRGET(UIElements)
};