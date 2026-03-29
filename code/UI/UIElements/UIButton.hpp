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
    void setText(Core::HashedString text) override;
    void setVisibility(bool visibility) override;

private:
	UIText* mText = nullptr;

public:
	GET(Text)
};
REGISTER_CLASS(UIButton);