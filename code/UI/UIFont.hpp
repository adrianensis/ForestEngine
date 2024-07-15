#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Material/Material.hpp"

class UIFont;

class UIFontsManager: public ObjectBase
{
    
public:
    void init();
    void terminate();
    Ptr<UIFont> loadFont(HashedString fontFile, u32 fontSize);
    Ptr<UIFont> getFont(HashedString fontFile) const;

private:
    FontsLibrary mFontsLibrary;
    std::unordered_map<HashedString, OwnerPtr<UIFont>> mFontsMap;
public:
    RGET(FontsLibrary)
};
REGISTER_CLASS(UIFontsManager);

class UIFont: public ObjectBase
{
    
public:
    void init(UIFontsManager& fontsManager, HashedString fontFile, u32 fontSize);
private:
    FontData mFontData;
    PoolHandler<Material> mFontMaterial;
public:
    GET(FontMaterial)
    CRGET(FontData)
};
REGISTER_CLASS(UIFont);
