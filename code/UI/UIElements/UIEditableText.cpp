#include "UI/UIElements/UIEditableText.hpp"

#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Window/Window.hpp"
#include "UI/UIManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Transform.hpp"


void UIEditableText::init() 
{
    UIPanel::init();

    mOnlyReleaseOnClickOutside = true;
}

void UIEditableText::initFromConfig(UIManager* uiManager, const UIElementConfig& config) 
{
	UIPanel::initFromConfig(uiManager, config);
    subscribeToMouseEvents();
    subscribeToEnterEvent();
    subscribeToEscEvent();
    subscribeToCharEvents();

	setText(mConfig.mText);
}
void UIEditableText::setText(Core::HashedString text)
{
	if (text.get().length() > 0)
	{
		if (!mText)
		{
			UIBuilder uiBuilder = mUIManager->createUIBuilder();

			mText = uiBuilder.
			setPosition(Maths::Vector2(-mConfig.mDisplaySize.x/2.0f, mConfig.mDisplaySize.y/2.0f)).
			setSize(mConfig.mDisplaySize).
			setText(text).
			setTextScale(mConfig.mTextScale).
			setLayer(mConfig.mLayer + 1).
			setIsAffectedByLayout(false).
			setParent(this).
			create<UIText>(mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName).getInternalPointer()).
			getUIElement<UIText>();
		}

		mText->setText(text);
	}
}

void UIEditableText::setVisibility(bool visibility) 
{
	UIPanel::setVisibility(visibility);

	if (mText)
	{
		mText->setVisibility(visibility);
	}
}