#pragma once

#include "Core/Module.hpp"
#include "Graphics/Material/Material.hpp"

class UIFont;

class UIFontsManager: public ObjectBase
{
    
public:
    void init();
    void terminate();
    Ptr<UIFont> loadFont(const std::string& fontFile);
    Ptr<UIFont> getFont(const std::string& fontFile) const;

private:
    FontsLibrary mFontsLibrary;
    std::unordered_map<std::string, OwnerPtr<UIFont>> mFontsMap;
public:
    RGET(FontsLibrary)
};
REGISTER_CLASS(UIFontsManager);

class UIFont: public ObjectBase
{
    
public:
    void init(UIFontsManager& fontsManager, const std::string& fontFile);
private:
    FontData mFontData;
    PoolHandler<Material> mFontMaterial;
public:
    GET(FontMaterial)
    CRGET(FontData)
};
REGISTER_CLASS(UIFont);
