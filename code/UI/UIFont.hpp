#pragma once

#include "Core/Module.hpp"
#include "Graphics/Material/Material.hpp"

class UIFont;

class UIFontsManager: public ObjectBase
{
    GENERATE_METADATA(UIFontsManager)
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

class UIFont: public ObjectBase
{
    GENERATE_METADATA(UIFont)
public:
    void init(UIFontsManager& fontsManager, const std::string& fontFile);
private:
    FontData mFontData;
    Handler mFontMaterial;
public:
    GET(FontMaterial)
    CRGET(FontData)
};