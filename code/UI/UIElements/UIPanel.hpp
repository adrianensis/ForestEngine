#pragma once

#include "UI/UIElements/UIElement.hpp"

class UIArea: public UIElement
{
    GENERATE_METADATA(UIArea)
public:
    void initFromConfig(const UIElementConfig& config) override;

private:
    virtual void calculateConfig() override;
};

class UIPanel: public UIArea
{
    GENERATE_METADATA(UIPanel)
public:
    void initFromConfig(const UIElementConfig& config) override;
};
