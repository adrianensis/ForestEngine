#pragma once

#include "Core/Minimal.hpp"
#include "Core/Object/ObjectBase.hpp"
#include "Core/ECS/EntityHandler.hpp"

class UIElement;

class UIGroup: public ObjectBase
{
public:
    void init();
    void addUIElement(TypedEntityHandler<UIElement> uiElement);
    void removeUIElement(TypedEntityHandler<UIElement> uiElement);
    void destroyUIElement(TypedEntityHandler<UIElement> uiElement);
    void destroyAllUIElements();
    void setVisibility(bool visibility);

public:
	HashedString mName;
	bool mVisible = false;
	std::list<TypedEntityHandler<UIElement>> mUIElements;
	
	CRGET(Name)
	GET(Visible)
	CRGET(UIElements)
};
REGISTER_CLASS(UIGroup);
