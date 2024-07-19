#include "UI/UIBuilder.hpp"
#include "Graphics/Module.hpp"
#include "UI/UIManager.hpp"

UIBuilder::UIBuilder()
{
	mCurrentLayout = UILayout::HORIZONTAL;
	mMakeRelativeToLastConfig = false;

    mDefaultConfig.mMaterial = GET_SYSTEM(UIManager).getDefaultUIMaterial();
    mDefaultConfig.mStyle = &UIStyleManager::getInstance().getDefaultStyle();
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

void UIBuilder::registerUIElement(TypedEntityHandler<UIElement> uiElement)
{
	mCurrentUIElement = uiElement;

	if (mConfig.mGroup.get().length() > 0)
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

		mLastConfig = uiElement->getConfig();
	}
}

UILayout UIBuilder::getOppositeLayout(UILayout layout)
{
	return (UILayout)(((int)mCurrentLayout + 1) % 2);
}

Vector2 UIBuilder::calculateNextElementOffset(UILayout layout)
{
	Vector2 offset = Vector2::smZero;

	switch (layout)
	{
		case UILayout::HORIZONTAL:
		{
			offset = Vector2((mLastConfig.mDisplaySize.x + UIUtils::correctAspectRatio(mConfig.mSeparatorSize)), 0);
			break;
		}
		case UILayout::VERTICAL:
		{
			offset = Vector2(0, -(mLastConfig.mDisplaySize.y + mConfig.mSeparatorSize));
			break;
		}
	}

	return offset;
}

void UIBuilder::calculateConfig()
{
	if (mConfig.mIsAffectedByLayout && mMakeRelativeToLastConfig)
	{
		Vector2 offset = calculateNextElementOffset(mNewRowOrColumn ? getOppositeLayout(mCurrentLayout) : mCurrentLayout);
		mConfig.mPosition = mLastConfig.mPosition + offset;
	}
}
