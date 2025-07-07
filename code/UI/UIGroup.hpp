#pragma once

#include "Engine/Core.hpp"
#include "Engine/EntityComponent/Entity.hpp"

class UIElement;

class UIGroup
{
public:
    void init();
    void addUIElement(EC::TEntityPtr<UIElement> uiElement);
    void removeUIElement(EC::TEntityPtr<UIElement> uiElement);
    void destroyUIElement(EC::TEntityPtr<UIElement> uiElement);
    void destroyAllUIElements();
    void setVisibility(bool visibility);

public:
	Core::HashedString mName;
	bool mVisible = false;
	std::list<EC::TEntityPtr<UIElement>> mUIElements;
	
	CRGET(Name)
	GET(Visible)
	CRGET(UIElements)
};
REGISTER_CLASS(UIGroup);
