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

    GPUShaderData shaderData;
    shaderData.mMaxInstances = 500;
    GPUShaderPropertiesBlockUI shaderPropertiesBlockUI;
    shaderPropertiesBlockUI.mColor = Vector4(1,1,1,1);
    mDefaultUIShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderUI, GPUShaderPropertiesBlockUI>(shaderData, shaderPropertiesBlockUI);
}

void UIManager::terminate()
{
    mGroups.clear();
	UIStyleManager::deleteInstance();
    mFontsManager.terminate();
}

const FontGlyphData& UIManager::getGlyphData(char character) const
{
    const auto& font = mFontsManager.getFont(mDefaultFont);
    const auto& glyphsArray = font->getFontShader()->getGPUShaderData().mFontData.mGlyphs;
    const FontGlyphData& glyph = glyphsArray.at(character);
    return glyph;
}

WeakPtr<GPUShader> UIManager::getFontShader() const
{
    const auto& f = mFontsManager.getFont(mDefaultFont);
    return f->getFontShader();
}

void UIManager::setFocusedElement(TEntityPtr<UIElement> focusedElement)
{
	mFocusedElement = focusedElement;
}
