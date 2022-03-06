#pragma once

// CPP_IGNORE

#include "UI/UIText.hpp"

#include "UI/UIStyle.hpp"

class UIStyleEditableTextBackground: public UIStyle
{
    GENERATE_METADATA(UIStyleEditableTextBackground)
public:
	UIStyleEditableTextBackground()
	{
		mBackgroundColor = Vector4(0, 0, 0.2f, 1);
	}
};

class UIEditableText: public UIText
{
    GENERATE_METADATA(UIEditableText)
protected:
    void setBackground(const UIElementConfig& config) override;

public:
    void init() override;
};