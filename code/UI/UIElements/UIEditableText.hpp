#pragma once

#include "UI/UIElements/UIText.hpp"
#include "UI/UIStyle.hpp"


class UIStyleEditableTextBackground: public UIStyle
{
public:
	UIStyleEditableTextBackground()
	{
		mBackgroundColor = Vector4(0, 0, 0.2f, 1);
	}
};
REGISTER_CLASS(UIStyleEditableTextBackground)

class UIEditableText: public UIText
{
    

public:
    void init() override;
    
protected:
    void setBackground(const UIElementConfig& config) override;
};
REGISTER_CLASS(UIEditableText);
