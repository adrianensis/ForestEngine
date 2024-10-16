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
    void setText(HashedString text) override;
    void setVisibility(bool visibility) override;
    
protected:
    void setIsEditable(bool editable);
    virtual void setBackground(const UIElementConfig& config) { };
private:
	i32 mLayer = 0;
	HashedString mString;
	std::vector<TypedEntityHandler<UITextGlyph>> mFontRenderers;
    bool mIsEditable = false;

protected:
    TypedEntityHandler<UIPanel> mBackground;

public:
	GET(IsEditable)
	CRGET(String)
	SET(Layer)
};
REGISTER_CLASS(UIText);
