#ifndef UIEDITABLETEXT_HPP
#define UIEDITABLETEXT_HPP

#include "UI/UIElements/UIText.hpp"
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

public:
    void init() override;
    
protected:
    void setBackground(CR(UIElementConfig) config) override;
};

#endif