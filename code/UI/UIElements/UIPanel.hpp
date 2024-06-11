#pragma once

#include "UI/UIElements/UIElement.hpp"

class UIArea: public UIElement
{
    
public:
    void initFromConfig(const UIElementConfig& config) override;
    virtual UIElementConfig calculateConfig(const UIElementConfig& config) override;
};
REGISTER_CLASS(UIArea);

class UIPanel: public UIArea
{
    
public:
    void initFromConfig(const UIElementConfig& config) override;
};
REGISTER_CLASS(UIPanel);
