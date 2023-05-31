#pragma once

#include "UI/UIElements/UIElement.hpp"


class UIPanel;

class UIText: public UIElement
{
    GENERATE_METADATA(UIText)

public:
    UIText();

    void init() override;
    void initFromConfig(const UIElementConfig& config) override;
    void onDestroy() override;
    void setText(const std::string& text) override;
    void setVisibility(bool visibility) override;
    
protected:
    void setIsEditable(bool editable);
    virtual void setBackground(const UIElementConfig& config) { };

private:
	i32 mLayer = 0;
	std::string mString;
	std::vector<Ptr<Renderer>> mFontRenderers;
    bool mIsEditable = false;

protected:
    Ptr<UIPanel> mBackground;

public:
	GET(IsEditable)
	CRGET(String)
	SET(Layer)
};
