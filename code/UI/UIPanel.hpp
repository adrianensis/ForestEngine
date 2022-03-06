#pragma once

// CPP_IGNORE

#include "UI/UIElement.hpp"

class UIPanel: public UIElement
{
    GENERATE_METADATA(UIPanel)
public:
    void initFromConfig(const UIElementConfig& config) override;
};