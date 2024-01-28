#include "UI/UIManager.hpp"
#include "UI/UIGroup.hpp"
#include "UI/UIStyle.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"

void UIManager::init()
{
	UIStyleManager::getInstance().init();
    mDefaultFont = Paths::mResources + "fonts/Arial.ttf";
    mFontsManager.init();
    mFontsManager.loadFont(mDefaultFont);

    MaterialData materialData;
    materialData.mAlphaEnabled = true;
    materialData.mReceiveLight = false;
    materialData.mUseColorAsTint = true;
    materialData.mUseDepth = true;
    mDefaultUIMaterial = GET_SYSTEM(MaterialManager).createMaterial(materialData);
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

Ptr<const MaterialFont> UIManager::getFontMaterial() const
{
    const auto& f = mFontsManager.getFont(mDefaultFont);
    return f->getFontMaterial();
}

void UIManager::setFocusedElement(Ptr<UIElement> focusedElement)
{
	mFocusedElement = focusedElement;
}
