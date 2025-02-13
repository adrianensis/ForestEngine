#include "UI/UIFont.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "UI/UIShader.hpp"

void UIFontsManager::init()
{
    mFontsLibrary.init();
}

void UIFontsManager::terminate()
{
    mFontsMap.clear();
    mFontsLibrary.terminate();
}

WeakPtr<UIFont> UIFontsManager::loadFont(HashedString fontFile, u32 fontSize)
{
    if(!mFontsMap.contains(fontFile))
    {
        mFontsMap.insert_or_assign(fontFile, OwnerPtr<UIFont>::newObject());
		WeakPtr<UIFont> font = mFontsMap.at(fontFile);
		font->init(*this, fontFile, fontSize);
    }

    return mFontsMap.at(fontFile);
}

WeakPtr<UIFont> UIFontsManager::getFont(HashedString fontFile) const
{
    return mFontsMap.at(fontFile);
}

void UIFont::init(UIFontsManager& fontsManager, HashedString fontFile, u32 fontSize)
{
    mFontData.loadFont(fontsManager.getFontsLibrary(), fontFile, fontSize);

    GPUShaderData shaderData;
    shaderData.mMaxInstances = 500;
    shaderData.mIsFont = true;
    shaderData.mGPUShaderTextureBindings.mTextureBindings.insert_or_assign(TextureBindingNames::smBaseColor, TextureBinding{mFontData.mPath});
    shaderData.mFontData = mFontData;
    GPUShaderPropertiesBlockUI shaderPropertiesBlockUI;
    shaderPropertiesBlockUI.mColor = Vector4(1,1,1,1);
    mFontShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderUIFont, GPUShaderPropertiesBlockUI>(shaderData, shaderPropertiesBlockUI);

    mFontData.freeGlyphsBuffers();
}