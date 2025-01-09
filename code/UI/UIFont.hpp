#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Shader/Shader.hpp"

class UIFont;

class UIFontsManager
{
    
public:
    void init();
    void terminate();
    WeakPtr<UIFont> loadFont(HashedString fontFile, u32 fontSize);
    WeakPtr<UIFont> getFont(HashedString fontFile) const;

private:
    FontsLibrary mFontsLibrary;
    std::unordered_map<HashedString, OwnerPtr<UIFont>> mFontsMap;
public:
    RGET(FontsLibrary)
};
REGISTER_CLASS(UIFontsManager);

class UIFont
{
    
public:
    void init(UIFontsManager& fontsManager, HashedString fontFile, u32 fontSize);
private:
    FontData mFontData;
    WeakPtr<Shader> mFontShader;
public:
    GET(FontShader)
    CRGET(FontData)
};
REGISTER_CLASS(UIFont);
