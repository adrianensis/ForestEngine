#pragma once

#include "Core/Module.hpp"
class UIElement;

class UIGroup: public ObjectBase
{
    GENERATE_METADATA(UIGroup)

public:
	std::string mName; GET_RC(Name)
	bool mVisible = false; GET(Visible)
	std::list<UIElement *> mUIElements; GET_RC(UIElements);
	
	void init();

	void addUIElement(UIElement * uiElement);
	void removeUIElement(UIElement * uiElement);
    void destroyUIElement(UIElement * uiElement);
	void destroyAllUIElements();
	void setVisibility(bool visibility);
};