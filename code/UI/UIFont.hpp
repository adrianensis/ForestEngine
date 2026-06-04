#pragma once

#include "Engine/Core.hpp"
#include "GPU/Shader/GPUShader.hpp"

class UIFont;

class UIFontsManager
{
    
public:
    void init();
    void terminate();
    Core::WeakPtr<UIFont> loadFont(Core::HashedString fontFile, Core::u32 fontSize);
    Core::WeakPtr<UIFont> getFont(Core::HashedString fontFile) const;

private:
    Font::FontsLibrary mFontsLibrary;
    std::unordered_map<Core::HashedString, Core::OwnerPtr<UIFont>> mFontsMap;
public:
    RGET(FontsLibrary)
};
REGISTER_CLASS(UIFontsManager);

class UIFont
{
    
public:
    void init(UIFontsManager& fontsManager, Core::HashedString fontFile, Core::u32 fontSize);
private:
    Font::FontData mFontData;
    GPUShader* mFontShader = nullptr;
public:
    GET(FontShader)
    CRGET(FontData)
};
REGISTER_CLASS(UIFont);
