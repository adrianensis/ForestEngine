#include "UI/UIElementConfig.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIStyle.hpp"
#include "Graphics/Module.hpp"


void UIElementConfig::init(const Vector2& position, const Vector2& size, i32 layer, PoolHandler<Material> material, HashedString text /*= std::string()*/)
{
	mStyle = &UIStyleManager::getInstance().getDefaultStyle();
	mUIElementClassId = 0;
	mPosition = position;
	mDisplayPosition = Vector2::smZero;
	mSize = size;
	mIsStatic = true;
	mText = text;
	mLayer = layer;
	// mTextSize = GET_SYSTEM(UIManager).getDefaultFontSize();
	mAdjustSizeToText = false;
	mIsAffectedByLayout = true;
	mMaterial = material;
	mGroup = "";
	mParent.invalidate();
	mSeparatorSize = 0.01f;
}
