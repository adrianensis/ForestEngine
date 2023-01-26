#pragma once

#include "UI/UIElements/UIPanel.hpp"


class UIText;

class UIButton: public UIPanel
{
    GENERATE_METADATA(UIButton)
	
public:
    void init() override;
    void initFromConfig(const UIElementConfig& config) override;
    void onDestroy() override;
    void onLabelDestroy();
    void setText(const std::string& text) override;
    void setVisibility(bool visibility) override;

private:
	UIText* mText = nullptr;

public:
	GET(Text)
};

class UIToggleButton: public UIButton
{
    GENERATE_METADATA(UIToggleButton)
public:
    void init() override;
};
