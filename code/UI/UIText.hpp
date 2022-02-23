#pragma once

#include "UI/UIElement.hpp"

class UIPanel;

class UIText: public UIElement
{
    GENERATE_METADATA(UIText)

private:
	i32 mLayer = 0;
	Vector2 mSize;
	std::string mString;
	std::vector<Renderer *> mFontRenderers;
    bool mIsEditable = false;

protected:
    UIPanel* mBackground = nullptr;
    
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