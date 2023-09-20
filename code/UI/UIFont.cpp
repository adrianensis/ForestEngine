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

Ptr<UIFont> UIFontsManager::loadFont(const std::string& fontFile)
{
    if(!MAP_CONTAINS(mFontsMap, fontFile))
    {
        MAP_INSERT(mFontsMap, fontFile, OwnerPtr<UIFont>::newObject());
		Ptr<UIFont> font = mFontsMap.at(fontFile);
		font->init(*this, fontFile);
    }

    return mFontsMap.at(fontFile);
}

Ptr<UIFont> UIFontsManager::getFont(const std::string& fontFile) const
{
    return mFontsMap.at(fontFile);
}

void UIFont::init(UIFontsManager& fontsManager, const std::string& fontFile)
{
    mFontData.loadFont(fontsManager.getFontsLibrary(), fontFile);

    MaterialData materialData;
    materialData.mCreateMipMap = false;
    //materialData.mAlphaEnabled = false;
    materialData.mFontData = mFontData;
    mFontMaterial = GET_SYSTEM(MaterialManager).createMaterialFont(materialData);

    mFontData.freeGlyphsBuffers();
}