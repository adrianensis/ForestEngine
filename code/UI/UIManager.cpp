#include "UI/UIManager.hpp"
#include "UI/UIGroup.hpp"
#include "UI/UIStyle.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "Core/Config/Paths.hpp"

void UIManager::init()
{
	UIStyleManager::getInstance().init();
    mDefaultFont = HashedString(Paths::mResources.get() + "fonts/Arial.ttf");
    mFontsManager.init();
    mFontsManager.loadFont(mDefaultFont, 12);

    ShaderData shaderData;
    shaderData.mMaxInstances = 500;
    ShaderPropertiesBlockUI shaderPropertiesBlockUI;
    shaderPropertiesBlockUI.mColor = Vector4(1,1,1,1);
    mDefaultUIShader = GET_SYSTEM(ShaderManager).createShader<ShaderUI, ShaderPropertiesBlockUI>(shaderData, shaderPropertiesBlockUI);
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
    const auto& glyphsArray = font->getFontShader()->getShaderData().mFontData.mGlyphs;
    const FontGlyphData& glyph = glyphsArray.at(character);
    return glyph;
}

WeakPtr<Shader> UIManager::getFontShader() const
{
    const auto& f = mFontsManager.getFont(mDefaultFont);
    return f->getFontShader();
}

void UIManager::setFocusedElement(TEntityHandler<UIElement> focusedElement)
{
	mFocusedElement = focusedElement;
}
