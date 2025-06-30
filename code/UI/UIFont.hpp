#pragma once

#include "Core/Minimal.hpp"
#include "GPU/Shader/GPUShader.hpp"

class UIFont;

class UIFontsManager
{
    
public:
    void init();
    void terminate();
    WeakPtr<UIFont> loadFont(Core::HashedString fontFile, u32 fontSize);
    WeakPtr<UIFont> getFont(Core::HashedString fontFile) const;

private:
    FontsLibrary mFontsLibrary;
    std::unordered_map<Core::HashedString, OwnerPtr<UIFont>> mFontsMap;
public:
    RGET(FontsLibrary)
};
REGISTER_CLASS(UIFontsManager);

class UIFont
{
    
public:
    void init(UIFontsManager& fontsManager, Core::HashedString fontFile, u32 fontSize);
private:
    FontData mFontData;
    WeakPtr<GPUShader> mFontShader;
public:
    GET(FontShader)
    CRGET(FontData)
};
REGISTER_CLASS(UIFont);
