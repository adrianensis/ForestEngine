#pragma once

#include "UI/UIElements/UIPanel.hpp"
#include "UI/UIStyle.hpp"

class UIText;

class UIStyleEditableTextBackground: public UIStyle
{
public:
	UIStyleEditableTextBackground()
	{
		mBackgroundColor = Maths::Vector4(0, 0, 0.2f, 1);
	}
};
REGISTER_CLASS(UIStyleEditableTextBackground)

class UIEditableText: public UIPanel
{
public:
    void init() override;
	void initFromConfig(const UIElementConfig& config) override;
    void setText(Core::HashedString text) override;
    void setVisibility(bool visibility) override;

private:
	EC::TEntityPtr<UIText> mText;
};
REGISTER_CLASS(UIEditableText);
