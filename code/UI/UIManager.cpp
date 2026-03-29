#include "UI/UIManager.hpp"
#include "UI/UIGroup.hpp"
#include "UI/UIStyle.hpp"

#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Window/Window.hpp"
#include "Scene/Module.hpp"
#include "Engine/Paths.hpp"
#include "GPU/GPUInstance.hpp"
#include "Window/WindowManager.hpp"

void UIManager::init()
{
    mWindow = mSystemsDI.getSystem<Window::WindowManager>()->getMainWindow().getInternalPointer();
    
	UIStyleManager::getInstance().init();
    mDefaultFont = Core::HashedString(Paths::PredefinedPaths::mResources.get() + "fonts/DejaVuSansMono.ttf");
    mFontsManager.init();
    mFontsManager.loadFont(mDefaultFont, 14);

    GPUShaderData shaderData;
    shaderData.mMaxInstances = 500;
    GPUShaderPropertiesBlockUI shaderPropertiesBlockUI;
    shaderPropertiesBlockUI.mColor = Maths::Vector4(1,1,1,1);
    mDefaultUIShader = GPUInstance::getInstance().mGPUShaderManager->createShader<GPUShaderUI, GPUShaderPropertiesBlockUI>(GPUInstance::getInstance().mGPUContext, shaderData, shaderPropertiesBlockUI);
}

void UIManager::terminate()
{
    mGroups.clear();
	UIStyleManager::deleteInstance();
    mFontsManager.terminate();
}

const Font::FontGlyphData& UIManager::getGlyphData(char character) const
{
    auto font = mFontsManager.getFont(mDefaultFont);
    const auto& glyphsArray = font->getFontData().mGlyphs;
    const Font::FontGlyphData& glyph = glyphsArray.at(character);
    return glyph;
}

Core::WeakPtr<GPUShader> UIManager::getFontShader() const
{
    auto font = mFontsManager.getFont(mDefaultFont);
    return font->getFontShader();
}

Core::WeakPtr<const UIFont> UIManager::getFont() const
{
    auto font = mFontsManager.getFont(mDefaultFont);
    return font;
}

void UIManager::setFocusedElement(UIElement* focusedElement)
{
	mFocusedElement = focusedElement;
}
