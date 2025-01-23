#include "UI/UIFont.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
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

    ShaderData shaderData;
    shaderData.mMaxInstances = 500;
    shaderData.mIsFont = true;
    shaderData.mShaderTextureBindings.mTextureBindings.insert_or_assign(TextureBindingNames::smBaseColor, TextureBinding{mFontData.mPath});
    shaderData.mFontData = mFontData;
    shaderData.setSharedShaderPropertiesBlock<ShaderPropertiesBlockUI>();
    shaderData.mSharedShaderPropertiesBlockBuffer.get<ShaderPropertiesBlockUI>().mColor = Vector4(1,1,1,1);
    // TODO: share shader between all fonts?? only texture changes
    mFontShader = GET_SYSTEM(ShaderManager).createShader<ShaderUIFont>(shaderData);

    mFontData.freeGlyphsBuffers();
}