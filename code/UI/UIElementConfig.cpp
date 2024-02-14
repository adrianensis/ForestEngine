#include "UI/UIElementConfig.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIStyle.hpp"
#include "Graphics/Module.hpp"


void UIElementConfig::init(const Vector2& position, const Vector2& size, i32 layer, u32 materialId, std::string text /*= std::string()*/)
{
	mStyle = &UIStyleManager::getInstance().getDefaultStyle();
	mUIElementClassId = 0;
	mPosition = position;
	mDisplayPosition = Vector2(0, 0);
	mSize = size;
	mIsStatic = true;
	mText = text;
	mLayer = layer;
	// mTextSize = GET_SYSTEM(UIManager).getDefaultFontSize();
	mAdjustSizeToText = false;
	mIsAffectedByLayout = true;
	mMaterialId = materialId;
	mGroup = "";
	mParent.invalidate();
	mSeparatorSize = 0.01f;
}
