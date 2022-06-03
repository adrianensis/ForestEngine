#include "UI/UIBuilder.hpp"
#include "Graphics/Module.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIStyle.hpp"


UIBuilder::UIBuilder()
{
	mCurrentLayout = UILayout::HORIZONTAL;
	mMakeRelativeToLastConfig = false;
	mCurrentUIElement = nullptr;

	mDefaultConfig.init(Vector2(0, 0), Vector2(0, 0), 0);
	mConfig = mDefaultConfig;
}

UIBuilder& UIBuilder::nextRow()
{
	mLastConfig = mLayoutFirstUIElementConfig;
	mNewRowOrColumn = true;
	return *this;
}

UIBuilder& UIBuilder::nextColumn()
{
	return nextRow(); // NOTE : exactly the same code.
}

UIBuilder& UIBuilder::saveData()
{
	mConfigStack.push_front(mConfig);
	return *this;
}

UIBuilder& UIBuilder::restoreData()
{
	mConfig = mConfigStack.front();
	mConfigStack.pop_front();
	return *this;
}

UIBuilder& UIBuilder::create(const String& className)
{
	UIElement* uiElement = INSTANCE_BY_NAME(className, UIElement);
	mConfig.mUIElementClassId = uiElement->getClassId();

	calculateConfig();
	uiElement->initFromConfig(mConfig);

	registerUIElement(uiElement);

	return *this;
}

void UIBuilder::registerUIElement(UIElement *uiElement)
{
	mCurrentUIElement = uiElement;

	ScenesManager::getInstance().getCurrentScene()->addGameObject(mCurrentUIElement);

	if (mConfig.mGroup.length() > 0)
	{
		UIManager::getInstance().getOrCreateGroup(mConfig.mGroup).addUIElement(mCurrentUIElement);
	}

	if (mConfig.mIsAffectedByLayout)
	{
		if (mNewRowOrColumn)
		{
			mNewRowOrColumn = false;
			mLayoutFirstUIElementConfig = mConfig;
		}

		mMakeRelativeToLastConfig = true;

		mLastConfig = mConfig;
	}
}

UILayout UIBuilder::getOppositeLayout(UILayout layout)
{
	return (UILayout)(((int)mCurrentLayout + 1) % 2);
}

Vector2 UIBuilder::calculateNextElementOffset(UILayout layout)
{
	Vector2 offset = Vector2(0, 0);

	switch (layout)
	{
		case UILayout::HORIZONTAL:
		{
			offset = Vector2(mLastConfig.mSize.x / RenderContext::getAspectRatio() + mConfig.mSeparatorSize, 0);
			break;
		}
		case UILayout::VERTICAL:
		{
			offset = Vector2(0, -(mLastConfig.mSize.y + mConfig.mSeparatorSize));
			break;
		}
	}

	return offset;
}

void UIBuilder::calculateConfig()
{
	if (mConfig.mAdjustSizeToText)
	{
		mConfig.mSize.x = (mConfig.mTextSize.x* mConfig.mText.length());
		mConfig.mSize.y = mConfig.mTextSize.y;
	}

	if (mConfig.mIsAffectedByLayout && mMakeRelativeToLastConfig)
	{
		Vector2 offset = calculateNextElementOffset(mNewRowOrColumn ? getOppositeLayout(mCurrentLayout) : mCurrentLayout);
		mConfig.mPosition = mLastConfig.mPosition + offset;
	}

	mConfig.mDisplayPosition = mConfig.mPosition;

	if(mConfig.mUIElementClassId == UIText::getClassIdStatic() || mConfig.mUIElementClassId == UIEditableText::getClassIdStatic())
	{
		mConfig.mDisplayPosition.x += mConfig.mTextSize.x/RenderContext::getAspectRatio();
		mConfig.mDisplayPosition.y -= mConfig.mTextSize.y / 2.0f;
	}
	else
	{
		mConfig.mDisplayPosition.x += (mConfig.mSize.x/RenderContext::getAspectRatio()) / 2.0f;
		mConfig.mDisplayPosition.y -= mConfig.mSize.y / 2.0f;
	}
}
