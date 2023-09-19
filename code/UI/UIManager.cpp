#include "UI/UIManager.hpp"
#include "UI/UIGroup.hpp"
#include "UI/UIStyle.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"

void UIManager::init()
{
	UIStyleManager::getInstance().init();
    mFontsManager.init();
    mFontsManager.loadFont(Paths::mResources + "fonts/Arial.ttf");
}

void UIManager::terminate()
{
	TRACE()
    mGroups.clear();
	UIStyleManager::deleteInstance();
    mFontsManager.terminate();
}

Vector2 UIManager::getCharTextureCoordinates(char character)
{
    const auto& f = mFontsManager.loadFont(Paths::mResources + "fonts/Arial.ttf");
    const auto& g = f->getFontMaterial()->getMaterialData().mFontData.mGlyphs;
    const FontGlyphData& glyph = g.at(character);
    return Vector2(glyph.xOffset, 0);
}

Vector2 UIManager::getFontTileTextureSize(char character)
{
    const auto& f = mFontsManager.loadFont(Paths::mResources + "fonts/Arial.ttf");
    const auto& g = f->getFontMaterial()->getMaterialData().mFontData.mGlyphs;
    const FontGlyphData& glyph = g.at(character);
    return glyph.mBitmapSize / Vector2(f->getFontData().mWidth, f->getFontData().mHeight);
}

Ptr<const MaterialFont> UIManager::getFontMaterial()
{
    const auto& f = mFontsManager.loadFont(Paths::mResources + "fonts/Arial.ttf");
    return f->getFontMaterial();
}

void UIManager::setFocusedElement(Ptr<UIElement> focusedElement)
{
	mFocusedElement = focusedElement;
}
