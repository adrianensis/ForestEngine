#pragma once

#include "Core/Minimal.hpp"
#include "Core/EntityComponent/Entity.hpp"

class UIElement;

class UIGroup
{
public:
    void init();
    void addUIElement(TEntityPtr<UIElement> uiElement);
    void removeUIElement(TEntityPtr<UIElement> uiElement);
    void destroyUIElement(TEntityPtr<UIElement> uiElement);
    void destroyAllUIElements();
    void setVisibility(bool visibility);

public:
	HashedString mName;
	bool mVisible = false;
	std::list<TEntityPtr<UIElement>> mUIElements;
	
	CRGET(Name)
	GET(Visible)
	CRGET(UIElements)
};
REGISTER_CLASS(UIGroup);
