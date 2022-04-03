#pragma once

#include "UI/UIText.hpp"
#include "UI/UIStyle.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIEditableText.hpp"
#include "Graphics/Module.hpp"
#include "UI/UIManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Transform.hpp"
#endif

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

protected:
    CPP void setBackground(const UIElementConfig& config) override
    {
        UIText::setBackground(config);
        
        if(mBackground)
        {
            getScene()->removeGameObject(mBackground);
        }

        UIBuilder uiBuilder;

        mBackground = uiBuilder.
            setPosition(getTransform().get().getWorldPosition() + Vector2(-config.mTextSize.x/RenderContext::getAspectRatio(), config.mTextSize.y/2.0f)).
            setLayer(mConfig.mLayer).
            setIsAffectedByLayout(false).
            setStyle(&UIStyleManager::getInstance().getOrAddStyle<UIStyleEditableTextBackground>()).
            create<UIPanel>().
            getUIElement<UIPanel>();
    }

public:
    CPP void init() override
    {
        UIText::init();
        setIsEditable(true);
    }
};