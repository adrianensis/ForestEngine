#include "UI/UIBuilder.hpp"
#include "Graphics/Module.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIStyle.hpp"


UIBuilder::UIBuilder()
{
	mCurrentLayout = UILayout::HORIZONTAL;
	mMakeRelativeToLastConfig = false;

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

void UIBuilder::registerUIElement(Ptr<UIElement> uiElement)
{
	mCurrentUIElement = uiElement;

	if (mConfig.mGroup.length() > 0)
	{
		GET_SYSTEM(UIManager).getOrCreateGroup(mConfig.mGroup).addUIElement(mCurrentUIElement);
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
			offset = Vector2((mLastConfig.mSize.x + mConfig.mSeparatorSize) / GET_SYSTEM(RenderContext).getAspectRatio(), 0);
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

	if (mConfig.mIsAffectedByLayout and mMakeRelativeToLastConfig)
	{
		Vector2 offset = calculateNextElementOffset(mNewRowOrColumn ? getOppositeLayout(mCurrentLayout) : mCurrentLayout);
		mConfig.mPosition = mLastConfig.mPosition + offset;
	}

	mConfig.mDisplayPosition = mConfig.mPosition;

	mConfig.mDisplayPosition.x += (mConfig.mSize.x/GET_SYSTEM(RenderContext).getAspectRatio()) / 2.0f;
    mConfig.mDisplayPosition.y -= mConfig.mSize.y / 2.0f;
}
