#pragma once

#include "UI/UIElements/UIPanel.hpp"

class UIPanel;

class UITextGlyph: public UIArea
{
public:
    void initFromConfig(const UIElementConfig& config) override;

private:
    char mCharacter = 0;
};
REGISTER_CLASS(UITextGlyph);

class UIText: public UIArea
{
public:
    void initFromConfig(const UIElementConfig& config) override;
    void onDestroy() override;
    void setText(Core::HashedString text) override;
    void setVisibility(bool visibility) override;
    
protected:
    void setIsEditable(bool editable);
    virtual void setBackground(const UIElementConfig& config) { };
private:
	Core::i32 mLayer = 0;
	Core::HashedString mString;
	std::vector<TEntityPtr<UITextGlyph>> mFontRenderers;
    bool mIsEditable = false;

protected:
    TEntityPtr<UIPanel> mBackground;

public:
	GET(IsEditable)
	CRGET(String)
	SET(Layer)
};
REGISTER_CLASS(UIText);
