#include "UI/UIFont.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/GPUInstance.hpp"
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

Core::WeakPtr<UIFont> UIFontsManager::loadFont(Core::HashedString fontFile, Core::u32 fontSize)
{
    if(!mFontsMap.contains(fontFile))
    {
        mFontsMap.insert_or_assign(fontFile, Core::OwnerPtr<UIFont>::newObject());
		Core::WeakPtr<UIFont> font = mFontsMap.at(fontFile);
		font->init(*this, fontFile, fontSize);
    }

    return mFontsMap.at(fontFile);
}

Core::WeakPtr<UIFont> UIFontsManager::getFont(Core::HashedString fontFile) const
{
    return mFontsMap.at(fontFile);
}

void UIFont::init(UIFontsManager& fontsManager, Core::HashedString fontFile, Core::u32 fontSize)
{
    mFontData.loadFont(fontsManager.getFontsLibrary(), fontFile, fontSize);

    GPUShaderData shaderData;
    shaderData.mMaxInstances = 500;
    shaderData.mIsFont = true;
    shaderData.mGPUShaderTextureBindings.mTextureBindings.insert_or_assign(TextureBindingNames::smBaseColor, TextureBinding{mFontData.mPath});
    shaderData.mFontData = mFontData;
    GPUShaderPropertiesBlockUI shaderPropertiesBlockUI;
    shaderPropertiesBlockUI.mColor = Maths::Vector4(1,1,1,1);
    mFontShader = GPUInstance::getInstance().mGPUShaderManager->createShader<GPUShaderUIFont, GPUShaderPropertiesBlockUI>(GPUInstance::getInstance().mGPUContext, shaderData, shaderPropertiesBlockUI);

    mFontData.freeGlyphsBuffers();
}