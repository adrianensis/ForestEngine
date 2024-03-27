#include "UI/UIFont.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/Material/MaterialManager.hpp"

void UIFontsManager::init()
{
    mFontsLibrary.init();
}

void UIFontsManager::terminate()
{
    mFontsMap.clear();
    mFontsLibrary.terminate();
}

Ptr<UIFont> UIFontsManager::loadFont(const std::string& fontFile, u32 fontSize)
{
    if(!mFontsMap.contains(fontFile))
    {
        mFontsMap.insert_or_assign(fontFile, OwnerPtr<UIFont>::newObject());
		Ptr<UIFont> font = mFontsMap.at(fontFile);
		font->init(*this, fontFile, fontSize);
    }

    return mFontsMap.at(fontFile);
}

Ptr<UIFont> UIFontsManager::getFont(const std::string& fontFile) const
{
    return mFontsMap.at(fontFile);
}

void UIFont::init(UIFontsManager& fontsManager, const std::string& fontFile, u32 fontSize)
{
    mFontData.loadFont(fontsManager.getFontsLibrary(), fontFile, fontSize);

    MaterialData materialData;
    materialData.mCreateMipMap = false;
    materialData.mReceiveLight = false;
    materialData.mUseColorAsTint = true;
    materialData.mUseDepth = true;
    // materialData.mAlphaEnabled = false;
    materialData.mIsFont = true;
    materialData.mFontData = mFontData;
    materialData.mSharedMaterialInstancedPropertiesBuffer.get<MaterialInstancedProperties>().mDiffuse = Vector4(1,1,1,1);
    mFontMaterial = GET_SYSTEM(MaterialManager).createMaterial(materialData);

    mFontData.freeGlyphsBuffers();
}