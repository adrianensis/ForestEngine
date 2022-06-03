#ifndef UIGROUP_HPP
#define UIGROUP_HPP

#include "Core/Module.hpp"


class UIElement;

class UIGroup: public ObjectBase
{
    GENERATE_METADATA(UIGroup)

public:
    void init();
    void addUIElement(UIElement *uiElement);
    void removeUIElement(UIElement *uiElement);
    void destroyUIElement(UIElement *uiElement);
    void destroyAllUIElements();
    void setVisibility(bool visibility);

public:
	String mName;
	bool mVisible = false;
	std::list<UIElement *> mUIElements;
	
	CRGET(Name)
	GET(Visible)
	CRGET(UIElements)
};

#endif