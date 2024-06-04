#include "UI/UIManager.hpp"
#include "UI/UIGroup.hpp"
#include "UI/UIStyle.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "Engine/Config/Paths.hpp"

void UIManager::init()
{
	UIStyleManager::getInstance().init();
    mDefaultFont = HashedString(Paths::mResources.get() + "fonts/Arial.ttf");
    mFontsManager.init();
    mFontsManager.loadFont(mDefaultFont, 12);

    MaterialData materialData;
    materialData.mReceiveLight = false;
    materialData.mCastShadows = false;
    materialData.mMaxInstances = 500;
    materialData.mReceiveShadows = false;
    materialData.mSharedMaterialPropertiesBlockBuffer.set<MaterialPropertiesBlockUI>();
    mDefaultUIMaterial = GET_SYSTEM(MaterialManager).createMaterial<ShaderUI>(materialData);
}

void UIManager::terminate()
{
	LOG_TRACE()
    mGroups.clear();
	UIStyleManager::deleteInstance();
    mFontsManager.terminate();
}

const FontGlyphData& UIManager::getGlyphData(char character) const
{
    const auto& font = mFontsManager.getFont(mDefaultFont);
    const auto& glyphsArray = font->getFontMaterial()->getMaterialData().mFontData.mGlyphs;
    const FontGlyphData& glyph = glyphsArray.at(character);
    return glyph;
}

PoolHandler<Material> UIManager::getFontMaterial() const
{
    const auto& f = mFontsManager.getFont(mDefaultFont);
    return f->getFontMaterial();
}

void UIManager::setFocusedElement(Ptr<UIElement> focusedElement)
{
	mFocusedElement = focusedElement;
}
