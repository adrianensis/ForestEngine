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
    if(mBackground)
    {
        mScene->removeGameObject(mBackground);
    }

    UIBuilder uiBuilder;

    mBackground = uiBuilder.
        // setPosition(mTransform->getWorldPosition() + Vector2(-config.mTextScale.x/GET_SYSTEM(WindowManager).getMainWindow()->getAspectRatio(), config.mTextScale.y/2.0f)).
        setPosition(Vector2(-mConfig.mDisplaySize.x/2.0f, mConfig.mDisplaySize.y/2.0f)).
        setSize(mConfig.mDisplaySize).
        setLayer(mConfig.mLayer).
        setIsAffectedByLayout(false).
        setParent(getPtrToThis<UIEditableText>()).
        setStyle(&UIStyleManager::getInstance().getOrAddStyle<UIStyleEditableTextBackground>()).
        create<UIPanel>().
        getUIElement<UIPanel>();
}
