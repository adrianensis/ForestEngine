#pragma once

#include "Engine/Core.hpp"
#include "Core/EntityComponent/Entity.hpp"

class UIElement;

class UIGroup
{
public:
    void init();
    void addUIElement(UIElement* uiElement);
    void removeUIElement(UIElement* uiElement);
    void destroyUIElement(UIElement* uiElement);
    void destroyAllUIElements();
    void setVisibility(bool visibility);

public:
	Core::HashedString mName;
	bool mVisible = false;
	std::list<UIElement*> mUIElements;
	
	CRGET(Name)
	GET(Visible)
	CRGET(UIElements)
};
REGISTER_CLASS(UIGroup);
