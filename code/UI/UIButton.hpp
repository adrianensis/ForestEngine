#pragma once

#include "UI/UIElement.hpp"

class UIText;

class UIButton: public UIElement
{
    GENERATE_METADATA(UIButton)
	
private:
	UIText* mText = nullptr;

public:
	void init() override;
	void initFromConfig(const UIElementConfig& config) override;
	void onDestroy() override;

	void onLabelDestroy();

	void setText(const std::string& text) override;
	void setVisibility(bool visibility) override;

	GET(Text)
};

class UIToggleButton: public UIButton
{
    GENERATE_METADATA(UIToggleButton)
public:
	void init() override;
};