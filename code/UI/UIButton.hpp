#pragma once

#include "UI/UIElement.hpp"

class UIText;

class UIButton: public UIElement
{
    GENERATE_METADATA(UIButton)
	PRI UIText* mText = nullptr; GET(Text)

public:
	void init() override;
	void initFromConfig(const UIElementConfig& config) override;
	void onDestroy() override;

	void onLabelDestroy();

	void setText(const std::string& text) override;
	void setVisibility(bool visibility) override;
};

class UIToggleButton: public UIButton
{
    GENERATE_METADATA(UIToggleButton)
public:
	void init() override;
};