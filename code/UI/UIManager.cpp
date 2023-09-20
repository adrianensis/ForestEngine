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
}

void UIManager::terminate()
{
	TRACE()
    mGroups.clear();
	UIStyleManager::deleteInstance();
    mFontsManager.terminate();
}

Rectangle UIManager::getGlyphTextureRegion(char character) const
{
    const auto& font = mFontsManager.getFont(mDefaultFont);
    const auto& glyphsArray = font->getFontMaterial()->getMaterialData().mFontData.mGlyphs;
    const FontGlyphData& glyph = glyphsArray.at(character);
    Vector2 glyphSizeInAtlasSpace = glyph.mBitmapSize / Vector2(font->getFontData().mWidth, font->getFontData().mHeight);
    return Rectangle(Vector2(glyph.xOffset, 0), glyphSizeInAtlasSpace);
}

Vector2 UIManager::getGlyphSize(char character) const
{
    const auto& font = mFontsManager.getFont(mDefaultFont);
    const auto& glyphsArray = font->getFontMaterial()->getMaterialData().mFontData.mGlyphs;
    const FontGlyphData& glyph = glyphsArray.at(character);
    return glyph.mBitmapSize;
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
