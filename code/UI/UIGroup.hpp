#pragma once

#include "Core/Minimal.hpp"


class UIElement;

class UIGroup: public ObjectBase
{
public:
    void init();
    void addUIElement(Ptr<UIElement> uiElement);
    void removeUIElement(Ptr<UIElement> uiElement);
    void destroyUIElement(Ptr<UIElement> uiElement);
    void destroyAllUIElements();
    void setVisibility(bool visibility);

public:
	HashedString mName;
	bool mVisible = false;
	std::list<Ptr<UIElement>> mUIElements;
	
	CRGET(Name)
	GET(Visible)
	CRGET(UIElements)
};
REGISTER_CLASS(UIGroup);
