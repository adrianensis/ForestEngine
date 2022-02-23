#pragma once

#include "UI/UIElement.hpp"

class UIPanel;

class UIText: public UIElement
{
    GENERATE_METADATA(UIText)
	PRI i32 mLayer = 0;
	PRI Vector2 mSize;
	PRI std::string mString;
	PRI std::vector<Renderer *> mFontRenderers;
    PRI bool mIsEditable = false;
    PRO UIPanel* mBackground = nullptr;

PRO
    void setIsEditable(bool editable);
    virtual void setBackground(const UIElementConfig& config) { };

public:
    UIText();

	void init() override;
	void initFromConfig(const UIElementConfig& config) override;
	void onDestroy() override;

	void setText(const std::string& text) override;

    void setVisibility(bool visibility) override;

	GET(IsEditable)
	GET_RC(String)
	SET(Size)
	SET(Layer)
};