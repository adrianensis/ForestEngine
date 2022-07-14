#ifndef UITEXT_HPP
#define UITEXT_HPP

#include "UI/UIElements/UIElement.hpp"


class UIPanel;

class UIText: public UIElement
{
    GENERATE_METADATA(UIText)

public:
    UIText();

    void init() override;
    void initFromConfig(CR(UIElementConfig) config) override;
    void onDestroy() override;
    void setText(CR(std::string) text) override;
    void setVisibility(bool visibility) override;
    
protected:
    void setIsEditable(bool editable);
    virtual void setBackground(CR(UIElementConfig) config) { };

private:
	i32 mLayer = 0;
	Vector2 mSize;
	std::string mString;
	std::vector<Renderer *> mFontRenderers;
    bool mIsEditable = false;

protected:
    UIPanel* mBackground = nullptr;

public:
	GET(IsEditable)
	CRGET(String)
	SET(Size)
	SET(Layer)
};

#endif