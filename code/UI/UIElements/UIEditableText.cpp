#include "UI/UIElements/UIEditableText.hpp"
#include "Graphics/Module.hpp"
#include "UI/UIManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Transform.hpp"


void UIEditableText::init() 
{
    UIText::init();
    setIsEditable(true);
}

void UIEditableText::setBackground(const UIElementConfig& config) 
{
    UIText::setBackground(config);
    
    if(mBackground)
    {
        mScene.get().removeGameObject(Ptr<GameObject>::cast(mBackground));
    }

    UIBuilder uiBuilder;

    mBackground = uiBuilder.
        setPosition(mTransform.get().getWorldPosition() + Vector2(-config.mTextSize.x/RenderContext::getAspectRatio(), config.mTextSize.y/2.0f)).
        setLayer(mConfig.mLayer).
        setIsAffectedByLayout(false).
        setStyle(&UIStyleManager::getInstance().getOrAddStyle<UIStyleEditableTextBackground>()).
        create<UIPanel>().
        getUIElement<UIPanel>();
}
