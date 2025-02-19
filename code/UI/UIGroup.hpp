#pragma once

#include "Core/Minimal.hpp"
#include "Core/EntityComponent/EntityHandler.hpp"

class UIElement;

class UIGroup
{
public:
    void init();
    void addUIElement(TEntityHandler<UIElement> uiElement);
    void removeUIElement(TEntityHandler<UIElement> uiElement);
    void destroyUIElement(TEntityHandler<UIElement> uiElement);
    void destroyAllUIElements();
    void setVisibility(bool visibility);

public:
	HashedString mName;
	bool mVisible = false;
	std::list<TEntityHandler<UIElement>> mUIElements;
	
	CRGET(Name)
	GET(Visible)
	CRGET(UIElements)
};
REGISTER_CLASS(UIGroup);
