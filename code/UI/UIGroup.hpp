#pragma once

#include "Engine/Core.hpp"
#include "Core/EntityComponent/Entity.hpp"

class UIElement;

class UIGroup
{
public:
    void init();
    void addUIElement(EC::EntityPtr<UIElement> uiElement);
    void removeUIElement(EC::EntityPtr<UIElement> uiElement);
    void destroyUIElement(EC::EntityPtr<UIElement> uiElement);
    void destroyAllUIElements();
    void setVisibility(bool visibility);

public:
	Core::HashedString mName;
	bool mVisible = false;
	std::list<EC::EntityPtr<UIElement>> mUIElements;
	
	CRGET(Name)
	GET(Visible)
	CRGET(UIElements)
};
REGISTER_CLASS(UIGroup);
