#pragma once

// CPP_IGNORE

#include "Core/Module.hpp"
class UIElement;

class UIGroup: public ObjectBase
{
    GENERATE_METADATA(UIGroup)

public:
	std::string mName;
	bool mVisible = false;
	std::list<UIElement *> mUIElements;
	
	void init();

	void addUIElement(UIElement * uiElement);
	void removeUIElement(UIElement * uiElement);
    void destroyUIElement(UIElement * uiElement);
	void destroyAllUIElements();
	void setVisibility(bool visibility);

	CRGET(Name)
	GET(Visible)
	CRGET(UIElements)
};