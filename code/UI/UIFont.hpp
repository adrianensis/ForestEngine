#pragma once

#include "Core/Minimal.hpp"
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
    FontsLibrary mFontsLibrary;
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
    FontData mFontData;
    Core::WeakPtr<GPUShader> mFontShader;
public:
    GET(FontShader)
    CRGET(FontData)
};
REGISTER_CLASS(UIFont);
