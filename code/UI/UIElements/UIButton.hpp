#pragma once

#include "UI/UIElements/UIPanel.hpp"


class UIText;

class UIButton: public UIPanel
{
    
	
public:
    void init() override;
    void initFromConfig(const UIElementConfig& config) override;
    void onDestroy() override;
    void onLabelDestroy();
    void setText(const std::string& text) override;
    void setVisibility(bool visibility) override;

private:
	Ptr<UIText> mText;

public:
	GET(Text)
};
REGISTER_CLASS(UIButton);