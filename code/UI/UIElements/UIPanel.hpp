#pragma once

#include "UI/UIElements/UIElement.hpp"

class UIArea: public UIElement
{
    
public:
    void initFromConfig(const UIElementConfig& config) override;

private:
    virtual void calculateConfig() override;
};
REGISTER_CLASS(UIArea);

class UIPanel: public UIArea
{
    
public:
    void initFromConfig(const UIElementConfig& config) override;
};
REGISTER_CLASS(UIPanel);
