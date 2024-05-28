#include "UI/UIFont.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "UI/UIMaterial.hpp"

void UIFontsManager::init()
{
    mFontsLibrary.init();
}

void UIFontsManager::terminate()
{
    mFontsMap.clear();
    mFontsLibrary.terminate();
}

Ptr<UIFont> UIFontsManager::loadFont(HashedString fontFile, u32 fontSize)
{
    if(!mFontsMap.contains(fontFile))
    {
        mFontsMap.insert_or_assign(fontFile, OwnerPtr<UIFont>::newObject());
		Ptr<UIFont> font = mFontsMap.at(fontFile);
		font->init(*this, fontFile, fontSize);
    }

    return mFontsMap.at(fontFile);
}

Ptr<UIFont> UIFontsManager::getFont(HashedString fontFile) const
{
    return mFontsMap.at(fontFile);
}

void UIFont::init(UIFontsManager& fontsManager, HashedString fontFile, u32 fontSize)
{
    mFontData.loadFont(fontsManager.getFontsLibrary(), fontFile, fontSize);

    MaterialData materialData;
    materialData.mReceiveLight = false;
    materialData.mCastShadows = false;
    materialData.mReceiveShadows = false;
    materialData.mIsFont = true;
    materialData.mTextureBindings.insert_or_assign(TextureBindingNames::smBaseColor, TextureBinding{mFontData.mPath, GPUPipelineStage::FRAGMENT});
    materialData.mFontData = mFontData;
    materialData.mSharedMaterialPropertiesBlockBuffer.set<MaterialPropertiesBlockUI>();
    materialData.mSharedMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mColor = Vector4(1,1,1,1);
    mFontMaterial = GET_SYSTEM(MaterialManager).createMaterial<ShaderUI>(materialData);

    mFontData.freeGlyphsBuffers();
}