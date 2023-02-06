#pragma once

#include "Core/Module.hpp"


class UIElement;

class UIGroup: public ObjectBase
{
    GENERATE_METADATA(UIGroup)

public:
    void init();
    void addUIElement(Ptr<UIElement> uiElement);
    void removeUIElement(Ptr<UIElement> uiElement);
    void destroyUIElement(Ptr<UIElement> uiElement);
    void destroyAllUIElements();
    void setVisibility(bool visibility);

public:
	std::string mName;
	bool mVisible = false;
	std::list<Ptr<UIElement>> mUIElements;
	
	CRGET(Name)
	GET(Visible)
	CRGET(UIElements)
};
