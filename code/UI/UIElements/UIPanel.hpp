#ifndef UIPANEL_HPP
#define UIPANEL_HPP

#include "UI/UIElements/UIElement.hpp"


class UIPanel: public UIElement
{
    GENERATE_METADATA(UIPanel)
public:
    void initFromConfig(const UIElementConfig& config) override;
};

#endif