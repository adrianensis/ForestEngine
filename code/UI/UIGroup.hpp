#pragma once

#include "Core/Module.hpp"
class UIElement;

class UIGroup: public ObjectBase
{
    GENERATE_METADATA(UIGroup)
	PUB std::string mName; GET_RC(Name)
	PUB bool mVisible = false; GET(Visible)
	PUB std::list<UIElement *> mUIElements; GET_RC(UIElements);

PUB
	void init();

	void addUIElement(UIElement * uiElement);
	void removeUIElement(UIElement * uiElement);
    void destroyUIElement(UIElement * uiElement);
	void destroyAllUIElements();
	void setVisibility(bool visibility);
};