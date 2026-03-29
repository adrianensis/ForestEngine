#pragma once

#include "UI/UIElements/UIPanel.hpp"

class UIPanel;

class UITextGlyph: public UIArea
{
public:
    void initFromConfig(UIManager* uiManager, const UIElementConfig& config) override;

private:
    char mCharacter = 0;
};
REGISTER_CLASS(UITextGlyph);

class UIText: public UIArea
{
public:
    void initFromConfig(UIManager* uiManager, const UIElementConfig& config) override;
    void onDestroy() override;
    void setText(Core::HashedString text) override;
    void setVisibility(bool visibility) override;
    
private:
	Core::HashedString mString;
	std::vector<UITextGlyph*> mFontRenderers;

protected:
    UIPanel* mBackground = nullptr;

public:
	CRGET(String)
};
REGISTER_CLASS(UIText);
